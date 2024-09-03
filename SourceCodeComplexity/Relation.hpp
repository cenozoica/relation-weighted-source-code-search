//
//  Relation.hpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 30.08.24.
//

#ifndef Relation_hpp
#define Relation_hpp

#include <vector>
#include <string>
#include <set>

// sequence of tokens as strings
struct RelationNaive {
    int pos;
    std::vector<std::string> tokenList;
    RelationNaive() : pos(-1) {}
};

// sequence of tokens as iterators
struct RelationCompressed {
    int pos;
    std::vector<unsigned int> tokenList;
    RelationCompressed(const RelationNaive& relationNaive, const std::vector<std::string>& tokenList);
    void UpdateIndex(const std::vector<std::string>& tokenSetGlobal, const std::vector<std::string>& tokenListFile);
    bool Has(const unsigned int q) const; // TODO has it how many times? this matters for the weight
};

#endif /* Relation_hpp */
