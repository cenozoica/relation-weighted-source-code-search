//
//  FileDescriptor.cpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 21.08.24.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "FileDescriptor.hpp"
#include "ParserCC.hpp"
#include "ParserJava.hpp"

// allowed file extensions
static const std::vector<std::string> fileExtAllowedVect = {
    ".h", ".hpp", ".c", ".cpp", ".java"
};

FileDescriptor::FileDescriptor(const std::filesystem::path& path)
: path(path), analyzed(false), fileHighLevelRep(nullptr)
{
}

bool FileDescriptor::IsFileExtensionAllowed() const
{
    for (const auto& fileExtAllowed : fileExtAllowedVect) {
        if (this->path.extension() == fileExtAllowed) {
            return true;
        }
    }
    return false;
}

std::unique_ptr<ParserBase> FileDescriptor::CreateParser()
{
    std::unique_ptr<ParserBase> result = nullptr;
    // not using RTTI or a map of file extension string -> parser classes/objects
    const std::string& s = this->path.extension();
    if (".h" == s || ".hpp" == s || ".c" == s || ".cpp" == s) {
        result = std::make_unique<ParserCC>();
    }
    else if (".java" == s) {
        result = std::make_unique<ParserJava>();
    }
    else {
        // ignored file
    }
    return result;
}

void FileDescriptor::Analyze()
{
    // check filename extension
    //bool fileExtAllowedFlag = IsFileExtensionAllowed();
    std::unique_ptr<ParserBase> parser = this->CreateParser();

    if (nullptr != parser) {
        this->fileHighLevelRep = std::make_unique<FileHighLevelRepresentation>();
        this->fileHighLevelRep->PreAnalyze();
        
        // transfer ownership of file high rep to parser
        parser->fileHighLevelRep = std::move(this->fileHighLevelRep);
        
        // read line by line
        std::ifstream fileHandle(this->path.c_str(), std::ios::in);
        if (fileHandle.is_open()) {
            while (true) {
                parser->SetLineStartPos(fileHandle.tellg()); // stream position to locate later the relation
                std::string line;
                if (std::getline(fileHandle, line)) {
                    if (line.length() > 0) {
                        parser->fileHighLevelRep->IncrementLineCount();
                    }
                    
                    for (const auto& c : line) {
                        parser->UpdateState(c);
                        
                        //this->simpleDebugOutput << parser->GetState();
                    }
                    parser->UpdateState('\0'); // signal eol
                }
                else {
                    break;
                }
            }
        }
        
        // check parse result
        if (parser->Check()) {
            // transfer ownership back
            this->fileHighLevelRep = std::move(parser->fileHighLevelRep);
            
            this->fileHighLevelRep->PostAnalyze();
        }
        else {
            std::cerr << this->path << "\tParsing failed:" << "\tstate consistency = " << parser->GetStateConsistency() << "\tdepth = " << parser->GetFHLRDepth() << std::endl;
        }
    }
}

void FileDescriptor::ToGlobalTokenSet(std::set<std::string>& tokenSet)
{
    if (nullptr != this->fileHighLevelRep) {
        this->fileHighLevelRep->ToGlobalTokenSet(tokenSet);
    }
}

void FileDescriptor::UpdateIndex(const std::vector<std::string>& tokenListGlobal)
{
    if (nullptr != this->fileHighLevelRep) {
        this->fileHighLevelRep->UpdateIndex(tokenListGlobal);
    }
}

std::tuple<int, int, int> FileDescriptor::GetComplexity() const
{
    if (nullptr != this-> fileHighLevelRep) {
        return {this->fileHighLevelRep->GetLineCount(), this->fileHighLevelRep->GetCommentEnergy(), this->fileHighLevelRep->GetRelationEnergy()};
    }
    else {
        return {0, 0, 0};
    }
}

void FileDescriptor::Search(const unsigned int q, const float weight)
{
    if (nullptr != this->fileHighLevelRep) {
        this->fileHighLevelRep->Search(q, this->searchResult);
        
        // calculate search result energy
        for (auto& relationCompressed : this->searchResult) {
            this->searchResultEnergy += weight * static_cast<float>(relationCompressed.tokenList.size());
        }
    }
}
