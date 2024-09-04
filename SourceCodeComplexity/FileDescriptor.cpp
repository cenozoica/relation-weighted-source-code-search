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
        this->fileHighLevelRep = std::make_unique<FileHighLevelRepresentation>();
        result = std::make_unique<ParserCC>(*this->fileHighLevelRep);
    }
    else if (".java" == s) {
        this->fileHighLevelRep = std::make_unique<FileHighLevelRepresentation>();
        result = std::make_unique<ParserJava>(*this->fileHighLevelRep);
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
        this->fileHighLevelRep->PreAnalyze();
        
        // read line by line
        std::ifstream fileHandle(this->path.c_str(), std::ios::in);
        if (fileHandle.is_open()) {
            while (true) {
                parser->SetLineStartPos(fileHandle.tellg()); // stream position to locate later the relation
                std::string line;
                if (std::getline(fileHandle, line)) {
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

void FileDescriptor::Index(const std::vector<std::string>& tokenListGlobal)
{
    if (nullptr != this->fileHighLevelRep) {
        this->fileHighLevelRep->Index(tokenListGlobal);
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
        for (const auto& relation : this->searchResult) {
            const std::vector<unsigned int>& tl = std::get<std::vector<unsigned int>>(relation.tokenList);
            this->searchResultEnergy += weight * tl.size();
        }
    }
}
