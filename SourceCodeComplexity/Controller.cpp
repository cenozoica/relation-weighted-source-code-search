//
//  Controller.cpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 28.08.24.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>

#include "Controller.hpp"

Controller::Controller(const std::vector<std::filesystem::path>& filelist)
: tokenList(nullptr)
{
    this->fileDescriptorList.reserve(filelist.size());
    for (auto& path : filelist) {
        this->fileDescriptorList.push_back(FileDescriptor(path));
    }
}

void Controller::Analyze()
{
    for (auto& fd : this->fileDescriptorList) {
        fd.Analyze();
    }
}

void Controller::AnalyzeParallel(SimpleDebugOutput& simpleDebugOutput)
{
    const auto t_start = std::chrono::high_resolution_clock::now();
    const auto processorCount = std::thread::hardware_concurrency();
    std::cout << "Processor count: " << processorCount << std::endl;
    std::vector<std::thread> workers;
    std::mutex fileAnalyzedMutex;

    for (int processorIndex = 0; processorIndex < processorCount; ++processorIndex) {
        workers.push_back(std::thread([this, &fileAnalyzedMutex, &simpleDebugOutput]() {
            for (auto& fd : this->fileDescriptorList) {
                fileAnalyzedMutex.lock();
                if (!fd.IsAnalyzed()) {
                    fd.SetAnalyzed(); // for other threads this file is as analyzed
                    fileAnalyzedMutex.unlock();
                    try {
                        fd.Analyze();
                    }
                    catch (const std::bad_alloc& e) {
                        std::cerr << e.what();
                    }
                }
                else {
                    fileAnalyzedMutex.unlock();
                }
            }
        }));
    }
    std::for_each(workers.begin(), workers.end(), [](std::thread &t) {
            t.join();
        });
    const auto t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Run time:\t" << std::setprecision(6) << std::chrono::duration<double, std::milli>(t_end - t_start) << std::endl;
}

void Controller::CreateUniqueTokenList()
{
    const auto t_start = std::chrono::high_resolution_clock::now();

    // create global set
    std::unique_ptr<std::set<std::string>> tokenSetTemp = std::make_unique<std::set<std::string>>();
    for (auto& fd : this->fileDescriptorList) {
        fd.ToGlobalTokenSet(*tokenSetTemp);
    }
    
    for (auto& fd : this->fileDescriptorList) {
        fd.ResetAnalyzed();
    }
    
    const auto processorCount = std::thread::hardware_concurrency();
    //std::cout << "Processor count: " << processorCount << std::endl;
    std::vector<std::thread> workers;
    std::mutex fileAnalyzedMutex;

    // update indexes
    std::unique_ptr<std::vector<std::string>> tokenListTemp = std::make_unique<std::vector<std::string>>(tokenSetTemp->begin(), tokenSetTemp->end());
    for (int processorIndex = 0; processorIndex < processorCount; ++processorIndex) {
        workers.push_back(std::thread([this, &fileAnalyzedMutex, &tokenListTemp]() {
            for (auto& fd : this->fileDescriptorList) {
                fileAnalyzedMutex.lock();
                if (!fd.IsAnalyzed()) {
                    fd.SetAnalyzed(); // for other threads this file is as analyzed
                    fileAnalyzedMutex.unlock();
                    fd.UpdateIndex(*tokenListTemp);
                }
                else {
                    fileAnalyzedMutex.unlock();
                }
            }
        }));
    }
    std::for_each(workers.begin(), workers.end(), [](std::thread &t) {
            t.join();
        });
    
    // transfer ownership to const
    this->tokenList = std::move(tokenListTemp);

    const auto t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Run time:\t" << std::setprecision(6) << std::chrono::duration<double, std::milli>(t_end - t_start) << std::endl;
}

void Controller::ToFileComplexity(const std::string filename) const
{
    std::ofstream file;
    file.open(filename, std::ofstream::out);
    if (file.is_open()) {
        for (auto& fd : this->fileDescriptorList) {
            file << fd.GetLineCount() << ' ' << fd.GetCommentEnergy() << ' ' << fd.GetRelationEnergy() << ' '<< std::setprecision(6) << fd.GetSearchResultEnergy() << std::endl;
        }
    }
    file.close();
}

void Controller::ToFileTokenSet(const std::string filename) const
{
    std::ofstream file;
    file.open(filename, std::ofstream::out);
    if (file.is_open()) {
        for (const auto& token : *this->tokenList) {
            file << token << ' ';
        }
    }
    file.close();
}

void Controller::Search(const std::string& q)
{
    const auto t_start = std::chrono::high_resolution_clock::now();

    this->searchToken.clear();
    const auto it = std::lower_bound(this->tokenList->begin(), this->tokenList->end(), q); // binary search
    if (it != this->tokenList->end()) {
        this->searchToken.push_back({static_cast<unsigned int>(std::distance(this->tokenList->begin(), it)), 1.0f}); // exact match
    }
    
    // search as substring
    for (unsigned int i = 0; i < this->tokenList->size(); ++i) {
        const auto& str = (*this->tokenList)[i];
        if (!str.empty()) {
            if (str.find(q) != std::string::npos) {
                // weight is substring length ratio
                const float weight = static_cast<float>(q.size()) / static_cast<float>(str.size());
                this->searchToken.push_back({i, weight});
            }
        }
    }
    
    for (auto& fd : this->fileDescriptorList) {
        fd.ResetAnalyzed();
    }
    
    const auto processorCount = std::thread::hardware_concurrency();
    //std::cout << "Processor count: " << processorCount << std::endl;
    std::vector<std::thread> workers;
    std::mutex fileAnalyzedMutex;
    
    for (int processorIndex = 0; processorIndex < processorCount; ++processorIndex) {
        workers.push_back(std::thread([this, &fileAnalyzedMutex]() {
            for (auto& fd : this->fileDescriptorList) {
                fileAnalyzedMutex.lock();
                if (!fd.IsAnalyzed()) {
                    fd.SetAnalyzed(); // for other threads this file is as analyzed
                    fileAnalyzedMutex.unlock();
                    fd.ResetSearchResult();
                    for (auto& q : this->searchToken) {
                        fd.Search(q.token, q.weight);
                    }
                }
                else {
                    fileAnalyzedMutex.unlock();
                }
            }
        }));
    }
    std::for_each(workers.begin(), workers.end(), [](std::thread &t) {
            t.join();
        });
    
    const auto t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Run time:\t" << std::setprecision(6) << std::chrono::duration<double, std::milli>(t_end - t_start) << std::endl;
}
