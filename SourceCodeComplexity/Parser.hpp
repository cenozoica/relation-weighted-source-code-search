//
//  Parser.hpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 19.08.24.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <stdio.h>
#include <memory>
#include "FileHighLevelRepresentation.hpp"

class ParserBase
{
public:
    enum class State {
        CODE,
        COMMENT,
        CHAR_LITERAL,
        STRING_LITERAL,
        TEXT_BLOCK
    };
protected:
    State state;
    int substateIndex;
    bool stateConsistency;
    int fhlrDepth;
    std::string token;
    RelationNaive relation;
    long long lineStartPos;
    
    void Transition(const State state, const int substateIndex = 0);
    virtual void UpdateStateStringLiteral(const char input);
    virtual void UpdateStateTextBlock(const char input) = 0;
    void CloseToken();
    void CloseRelation();
public:
    static const size_t TOKEN_RELATION_SIZE_MAX = 10;
    std::unique_ptr<FileHighLevelRepresentation> fileHighLevelRep; // yes
    ParserBase();
    virtual ~ParserBase() {}
    State GetState() const { return this->state; }
    int GetFHLRDepth() const { return this->fhlrDepth; }
    bool GetStateConsistency() const { return this->stateConsistency; }
    bool Check() const { return true == stateConsistency && 0 == this->fhlrDepth; }
    virtual void UpdateState(const char input);
    void SetLineStartPos(const long long pos) { this->lineStartPos = pos; }
};

#endif /* Parser_hpp */
