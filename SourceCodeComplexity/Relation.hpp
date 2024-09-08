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

// a relation is defined as a sequence of tokens
// what terminates a sequence of tokens is ;[](){}
// parent relations e.g. function call are not yet considered

// a relation can occupy one or more, but not many, lines
// this information helps print relation as full code lines
// since the number of relation instances is hundreds of thousands in memory this struct must be optimized

struct Relation {
    static const unsigned int POS_UNINIT = 1048570U;
    static const unsigned int LINE_COUNT_MAX = 15U;
    unsigned int pos: 20; // stream position (byte count <= 1MB)
    unsigned int lineCount: 4; // usually 1
    // 8 unused bits, not sure how variant below aligns

    // sequence of tokens as strings or indexes
    std::variant<std::vector<std::string>, std::vector<unsigned int>> tokenList;
    
    Relation() : pos(POS_UNINIT), lineCount(0) {}
    //Relation(const Relation& other) {}
    //~Relation() {}
    
    void Reset();
    void Index(const std::vector<std::string>& tokenListFile);
    void Index(const std::vector<std::string>& tokenListGlobal, const std::vector<std::string>& tokenListFile);
    bool Has(const unsigned int q) const; // TODO has it how many times? this matters for the weight
};

#endif /* Relation_hpp */
