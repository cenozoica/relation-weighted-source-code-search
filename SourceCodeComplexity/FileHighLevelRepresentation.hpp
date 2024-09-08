//
//  FileHighLevelRepresentation.hpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 26.08.24.
//

#ifndef FileHighLevelRepresentation_hpp
#define FileHighLevelRepresentation_hpp

#include <stdio.h>
#include <algorithm>
#include <vector>
#include <set>

#include "Relation.hpp"

class FileHighLevelRepresentation {
private:
    // simple complexity indicators
    int lineCount;
    int commentEnergy;
    int maxDepth; // refers to block [](){} nesting
    
    // tokens and relations
    std::unique_ptr<const std::vector<std::string>> tokenList;
    std::unique_ptr<std::vector<Relation>> relationList;
public:
    FileHighLevelRepresentation();
    FileHighLevelRepresentation(const FileHighLevelRepresentation& other) {}
    
    // parse methods
    void CloseRelation(const Relation& relation);
    void PreAnalyze();
    void PostAnalyze();
    void ToGlobalTokenSet(std::set<std::string>& tokenSetGlobal);
    void Index(const std::vector<std::string>& tokenListGlobal);

    // complexity getters
    int GetLineCount() const { return this->lineCount; }
    int GetMaxDepth() const { return this->maxDepth; }
    int GetCommentEnergy() const { return this->commentEnergy; }
    size_t GetRelationCount() const { return this->relationList->size(); }
    size_t GetRelationEnergy() const;

    // complexity setters
    void SetMaxDepth(const int value) { this->maxDepth = std::max(this->maxDepth, value); }
    void IncrementCommentEnergy() { this->commentEnergy++; }
    void IncrementLineCount() { this->lineCount++; }

    void Search(const unsigned int q, std::vector<unsigned int>& searchResult);
    const Relation& GetRelation(const unsigned int i) const { return (*this->relationList)[i]; }
    
    static void Test();
};

#endif /* FileHighLevelRepresentation_hpp */
