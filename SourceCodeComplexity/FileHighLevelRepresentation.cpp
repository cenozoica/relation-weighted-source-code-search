//
//  FileHighLevelRepresentation.cpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 26.08.24.
//

#include <iostream>
#include <memory>
#include <iomanip>

#include "FileHighLevelRepresentation.hpp"

FileHighLevelRepresentation::FileHighLevelRepresentation() :
lineCount(0), maxDepth(0), commentEnergy(0),
tokenList(nullptr), relationList(nullptr)
{
}

void FileHighLevelRepresentation::PreAnalyze()
{
    this->relationList = std::make_unique<std::vector<Relation>>();
}

void FileHighLevelRepresentation::PostAnalyze()
{
    // analyze cannot create compressed relations because the set of tokens is not available
    
    if (nullptr != this->relationList) {
        // create set of tokens
        std::unique_ptr<std::set<std::string>> tokenSetTemp = std::make_unique<std::set<std::string>>();
        for (const auto& rel : *this->relationList) {
            const std::vector<std::string>& tl = std::get<std::vector<std::string>>(rel.tokenList);
            tokenSetTemp->insert(tl.begin(), tl.end());
        }

        // from set to vector
        std::unique_ptr<std::vector<std::string>> tokenListTemp = std::make_unique<std::vector<std::string>>(tokenSetTemp->begin(), tokenSetTemp->end());
        tokenSetTemp->clear();
        
        // transfer ownership to const
        this->tokenList = std::move(tokenListTemp);
        
        // index relations
        for (auto& relation : *this->relationList) {
            relation.Index(*this->tokenList);
        }
    }
}

void FileHighLevelRepresentation::ToGlobalTokenSet(std::set<std::string>& tokenSetGlobal)
{
    if (nullptr != this->tokenList) {
        tokenSetGlobal.insert(this->tokenList->begin(), this->tokenList->end());
    }
}

void FileHighLevelRepresentation::Index(const std::vector<std::string>& tokenListGlobal)
{
    for (auto& relation : *this->relationList) {
        relation.Index(tokenListGlobal, *this->tokenList);
    }
    
    // loose token list
    this->tokenList = nullptr;
}

void FileHighLevelRepresentation::CloseRelation(const Relation& relation)
{
    this->relationList->push_back(relation);
}

size_t FileHighLevelRepresentation::GetRelationEnergy() const
{
    size_t result = 0;
    
    if (nullptr != this->relationList) {
        for (const auto& relation : *this->relationList) {
            if (std::holds_alternative<std::vector<unsigned int>>(relation.tokenList)) {
                result += std::get<std::vector<unsigned int>>(relation.tokenList).size();
            }
        }
    }
    
    return result;
}

void FileHighLevelRepresentation::Search(const unsigned int q, std::vector<unsigned int>& searchResult)
{
    if (nullptr != this->relationList) {
        for (unsigned int i = 0; i < this->relationList->size(); ++i) {
            if ((*this->relationList)[i].Has(q)) {
                searchResult.push_back(i);
            }
        }
    }
}

void FileHighLevelRepresentation::Test()
{
}
