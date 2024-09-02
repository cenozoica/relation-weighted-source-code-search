//
//  Relation.cpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 30.08.24.
//

#include <iostream>

#include "Relation.hpp"

RelationCompressed::RelationCompressed(const RelationNaive& relationNaive, const std::vector<std::string>& tokenList)
{
    this->line = relationNaive.line;
    
    for (const auto& token : relationNaive.tokenList) {
        const auto it = std::lower_bound(tokenList.begin(), tokenList.end(), token); // binary search
        if (it != tokenList.end()) {
            this->tokenList.push_back(static_cast<unsigned int>(std::distance(tokenList.begin(), it)));
        }
        else {
            std::cerr << "Could not find\t" << token << std::endl;
        }
    }
}

void RelationCompressed::UpdateIndex(const std::vector<std::string>& tokenSetGlobal, const std::vector<std::string>& tokenListFile)
{
    for (size_t i = 0; i < this->tokenList.size(); ++i) {
        const std::string& q = tokenListFile[this->tokenList[i]];
        const auto it = std::lower_bound(tokenSetGlobal.begin(), tokenSetGlobal.end(), q); // binary search
        if (it != tokenSetGlobal.end()) {
            this->tokenList[i] = static_cast<unsigned int>(std::distance(tokenSetGlobal.begin(), it));
        }
        else {
            std::cerr << "Could not find:\t"<< q << std::endl;
        }
    }
}

bool RelationCompressed::Has(const unsigned int q) const
{
    return std::find(this->tokenList.begin(), this->tokenList.end(), q) != this->tokenList.end();
}
