//
//  Relation.cpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 30.08.24.
//

#include <iostream>

#include "Relation.hpp"

void Relation::Index(const std::vector<std::string>& tokenListFile)
{
    const std::vector<std::string>& tl = std::get<std::vector<std::string>>(this->tokenList);
    std::vector<unsigned int> tokenListNew;
    for (const auto& token : tl) {
        const auto it = std::lower_bound(tokenListFile.begin(), tokenListFile.end(), token); // binary search
        if (it != tokenListFile.end()) {
            tokenListNew.push_back(static_cast<unsigned int>(std::distance(tokenListFile.begin(), it)));
        }
        else {
            std::cerr << "Could not find\t" << token << std::endl;
        }
    }
    this->tokenList = tokenListNew;
}

void Relation::Index(const std::vector<std::string>& tokenListGlobal, const std::vector<std::string>& tokenListFile)
{
    std::vector<unsigned int>& tl = std::get<std::vector<unsigned int>>(this->tokenList);
    for (size_t i = 0; i < tl.size(); ++i) {
        const std::string& q = tokenListFile[tl[i]];
        const auto it = std::lower_bound(tokenListGlobal.begin(), tokenListGlobal.end(), q); // binary search
        if (it != tokenListGlobal.end()) {
            tl[i] = static_cast<unsigned int>(std::distance(tokenListGlobal.begin(), it));
        }
        else {
            std::cerr << "Could not find:\t"<< q << std::endl;
        }
    }
}

bool Relation::Has(const unsigned int q) const
{
    const std::vector<unsigned int>& tl = std::get<std::vector<unsigned int>>(this->tokenList);
    return std::find(tl.begin(), tl.end(), q) != tl.end();
}
