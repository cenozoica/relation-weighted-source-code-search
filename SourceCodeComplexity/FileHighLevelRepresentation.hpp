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
    std::unique_ptr<std::vector<RelationNaive>> relationNaiveList;
    std::unique_ptr<std::vector<RelationCompressed>> relationCompressedList;
public:
    FileHighLevelRepresentation();
    FileHighLevelRepresentation(const FileHighLevelRepresentation& other) {}
    
    // parse methods
    void CloseRelation(RelationNaive& relationNaive);
    void PreAnalyze();
    void PostAnalyze();
    void ToGlobalTokenSet(std::set<std::string>& tokenSetGlobal);
    void UpdateIndex(const std::vector<std::string>& tokenListGlobal);

    // complexity getters
    int GetLineCount() const { return this->lineCount; }
    int GetMaxDepth() const { return this->maxDepth; }
    int GetCommentEnergy() const { return this->commentEnergy; }
    size_t GetRelationCount() const { return this->relationCompressedList->size(); }
    size_t GetRelationEnergy() const;

    // complexity setters
    void SetMaxDepth(const int value) { this->maxDepth = std::max(this->maxDepth, value); }
    void IncrementCommentEnergy() { this->commentEnergy++; }
    void IncrementLineCount() { this->lineCount++; }

    void Search(const unsigned int q, std::vector<RelationCompressed>& searchResult);
    
    static void Test();
};

#endif /* FileHighLevelRepresentation_hpp */
