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
    std::cout << "Run time file parse:\t" << std::setprecision(6) << std::chrono::duration<double, std::milli>(t_end - t_start) << std::endl;
}

void Controller::CreateUniqueTokenList()
{
    const auto t_start = std::chrono::high_resolution_clock::now();

    // create global set
    std::unique_ptr<std::set<std::string>> tokenSetTemp = std::make_unique<std::set<std::string>>();
    for (auto& fd : this->fileDescriptorList) {
        fd.ToGlobalTokenSet(*tokenSetTemp);
    }
    
    std::unique_ptr<std::vector<std::string>> tokenListTemp = std::make_unique<std::vector<std::string>>(tokenSetTemp->begin(), tokenSetTemp->end());
    tokenSetTemp->clear();

    for (auto& fd : this->fileDescriptorList) {
        fd.ResetAnalyzed();
    }
    
    const auto processorCount = std::thread::hardware_concurrency();
    //std::cout << "Processor count: " << processorCount << std::endl;
    std::vector<std::thread> workers;
    std::mutex fileAnalyzedMutex;

    // update indexes
    for (int processorIndex = 0; processorIndex < processorCount; ++processorIndex) {
        workers.push_back(std::thread([this, &fileAnalyzedMutex, &tokenListTemp]() {
            for (auto& fd : this->fileDescriptorList) {
                fileAnalyzedMutex.lock();
                if (!fd.IsAnalyzed()) {
                    fd.SetAnalyzed(); // for other threads this file is as analyzed
                    fileAnalyzedMutex.unlock();
                    fd.Index(*tokenListTemp);
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
    std::cout << "Run time create unique token list:\t" << std::setprecision(6) << std::chrono::duration<double, std::milli>(t_end - t_start) << std::endl;
}

void Controller::ToFileComplexity(const std::string filename) const
{
    std::ofstream file;
    file.open(filename, std::ofstream::out);
    if (file.is_open()) {
        for (auto& fd : this->fileDescriptorList) {
            const auto [lineCount, commentEnergy, relationEnergy] = fd.GetComplexity();
            file << lineCount << ' ' << commentEnergy << ' ' << relationEnergy << ' '<< std::setprecision(6) << fd.GetSearchResultEnergy() << std::endl;
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

void Controller::CreateSearchTokenList(const std::string& q)
{
    const auto t_start = std::chrono::high_resolution_clock::now();

    this->searchToken.clear();
    
    // search exact match
    const auto it = std::lower_bound(this->tokenList->begin(), this->tokenList->end(), q); // binary search
    if (it != this->tokenList->end()) {
        this->searchToken.push_back({static_cast<unsigned int>(std::distance(this->tokenList->begin(), it)), 1.0f});
    }
    
    // search as substring
    // TODO parallel
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
    
    // search also q[0] capitalized but weight = 0.5
    if (std::islower(q[0])) {
        std::string r = q;
        r[0] = std::toupper(r[0]);
        // search exact match
        const auto it = std::lower_bound(this->tokenList->begin(), this->tokenList->end(), r); // binary search
        if (it != this->tokenList->end()) {
            this->searchToken.push_back({static_cast<unsigned int>(std::distance(this->tokenList->begin(), it)), 0.5f});
        }
    }
    
    const auto t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Run time search keyword index:\t" << std::setprecision(6) << std::chrono::duration<double, std::milli>(t_end - t_start) << std::endl;
}

void Controller::Search(const std::string& q)
{
    this->CreateSearchTokenList(q);
    
    for (auto& fd : this->fileDescriptorList) {
        fd.ResetAnalyzed();
    }
    this->searchResultTop.clear();
    this->searchResultTop.reserve(SEARCH_RESULT_TOP_SIZE_MAX);
    
    const auto t_start = std::chrono::high_resolution_clock::now();

    const auto processorCount = std::thread::hardware_concurrency();
    //std::cout << "Processor count: " << processorCount << std::endl;
    std::vector<std::thread> workers;
    std::mutex fileAnalyzedMutex;
    std::mutex searchResultMutex;

    for (int processorIndex = 0; processorIndex < processorCount; ++processorIndex) {
        workers.push_back(std::thread([this, &fileAnalyzedMutex, &searchResultMutex]() {
            for (int i = 0; i < this->fileDescriptorList.size(); ++i) {
                auto& fd = this->fileDescriptorList[i];
                fileAnalyzedMutex.lock();
                if (!fd.IsAnalyzed()) {
                    fd.SetAnalyzed(); // for other threads this file is as analyzed
                    fileAnalyzedMutex.unlock();
                    fd.ResetSearchResult();
                    for (auto& q : this->searchToken) {
                        fd.Search(q.token, q.weight);
                    }
                    if (!fd.GetSearchResult().empty()) {
                        // push to search result, bubble sort by energy
                        searchResultMutex.lock();
                        int j; // where this file must be inserted in the search result top
                        for (j = 0; j < this->searchResultTop.size(); ++j) {
                            if (fd.GetSearchResultEnergy() > this->fileDescriptorList[this->searchResultTop[j]].GetSearchResultEnergy()) {
                                break;
                            }
                        }
                        if (j < this->searchResultTop.size()) {
                            this->searchResultTop.insert(this->searchResultTop.begin() + j, i);
                        }
                        else {
                            this->searchResultTop.push_back(i);
                        }
                        this->searchResultTop.resize(SEARCH_RESULT_TOP_SIZE_MAX);
                        searchResultMutex.unlock();
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
    std::cout << "Run time file search:\t" << std::setprecision(6) << std::chrono::duration<double, std::milli>(t_end - t_start) << std::endl;
}

void Controller::ToFileSearchResult(const std::string filename) const
{
    std::ofstream f;
    f.open(filename);
    if (f.is_open()) {
        for (const auto& searchResultTopIndex : this->searchResultTop) {
            if (searchResultTopIndex >= 0) {
                const auto& fd = this->fileDescriptorList[searchResultTopIndex];
                const std::string& s = fd.GetPath().filename().string();
                
                // write header
                static const size_t LINE_LEN = 80;
                size_t i;
                for (i = 0; i < (LINE_LEN - s.size()) / 2; ++i) {
                    f << '=';
                }
                f << ' ' << ' ' << s << ' ' << ' ';
                i += s.size() + 2*2;
                for (; i < LINE_LEN; ++i) {
                    f << '=';
                }
                f << std::endl;
                
                // write findings
                
                std::ifstream g(fd.GetPath().c_str(), std::ios::in);
                long long prevRelationPos = -1;
                if (g.is_open()) {
                    const auto& relationList = fd.GetSearchResult();
                    for (auto it = relationList.begin(); it != relationList.end(); ++it) {
                        g.seekg(it->pos); // go to relation code
                        
                        bool aggregateCode = false; // relation lines can intersect
                        
                        if (aggregateCode) {
                            // TODO
                        }
                        else {
                            // write relation separator
                            if (prevRelationPos >= 0) {
                                for (size_t i = 0; i < LINE_LEN; ++i) {
                                    f << '-';
                                }
                                f << std::endl;
                            }
                            
                            // write relation lines
                            for (size_t lineIndex = 0; lineIndex < it->lineCount; ++lineIndex) {
                                std::string line;
                                if (std::getline(g, line)) {
                                    f << line << std::endl;
                                }
                                else {
                                    break;
                                }
                            }
                        }

                        prevRelationPos = g.tellg();
                    }
                }
                else {
                    f << "ERROR: Could not open file" << std::endl;
                }
            }
        }
    }
    f.close();
}
