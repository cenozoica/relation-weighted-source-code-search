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
tokenList(nullptr), relationNaiveList(nullptr), relationCompressedList(nullptr)
{
}

void FileHighLevelRepresentation::PreAnalyze()
{
    this->relationNaiveList = std::make_unique<std::vector<RelationNaive>>();
}

void FileHighLevelRepresentation::PostAnalyze()
{
    // analyze cannot create compressed relations because the set of tokens is not available
    
    if (nullptr != this->relationNaiveList) {
        // create set of tokens
        std::unique_ptr<std::set<std::string>> tokenSetTemp = std::make_unique<std::set<std::string>>();
        for (auto& rel : *this->relationNaiveList) {
            tokenSetTemp->insert(rel.tokenList.begin(), rel.tokenList.end());
        }

        // from set to vector
        std::unique_ptr<std::vector<std::string>> tokenListTemp = std::make_unique<std::vector<std::string>>(tokenSetTemp->begin(), tokenSetTemp->end());
        // transfer ownership to const
        this->tokenList = std::move(tokenListTemp);
        
        // create compressed relations
        this->relationCompressedList = std::make_unique<std::vector<RelationCompressed>>();
        for (const auto& relationNaive : *this->relationNaiveList) {
            this->relationCompressedList->push_back(RelationCompressed(relationNaive, *this->tokenList));
        }

        // loose naive relations
        this->relationNaiveList = nullptr;
    }
}

void FileHighLevelRepresentation::ToGlobalTokenSet(std::set<std::string>& tokenSetGlobal)
{
    tokenSetGlobal.insert(this->tokenList->begin(), this->tokenList->end());
}

void FileHighLevelRepresentation::UpdateIndex(const std::vector<std::string>& tokenListGlobal)
{
    for (auto& relationCompressed : *this->relationCompressedList) {
        relationCompressed.UpdateIndex(tokenListGlobal, *this->tokenList);
    }
    
    // loose token list
    this->tokenList = nullptr;
}

void FileHighLevelRepresentation::CloseRelation(RelationNaive& relationNaive)
{
    this->relationNaiveList->push_back(relationNaive);
}

size_t FileHighLevelRepresentation::GetRelationEnergy() const
{
    size_t result = 0;
    
    if (nullptr != this->relationCompressedList) {
        for (const auto& relationCompressed : *this->relationCompressedList) {
            result += relationCompressed.tokenList.size();
        }
    }
    
    return result;
}

void FileHighLevelRepresentation::Search(const unsigned int q, std::vector<RelationCompressed>& searchResult)
{
    if (nullptr != this->relationCompressedList) {
        for (const auto& relationCompressed : *this->relationCompressedList) {
            if (relationCompressed.Has(q)) {
                searchResult.push_back(relationCompressed);
            }
        }
    }
}

void FileHighLevelRepresentation::Test()
{
}
