//
//  Parser.cpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 19.08.24.
//

#include <iostream>
#include <string>

#include "Parser.hpp"

static const std::string NUMBER_LITERAL = "__NUMBER_LITERAL";

ParserBase::ParserBase(FileHighLevelRepresentation& fileHighLevelRep)
: state(State::CODE), substateIndex(0), stateConsistency(true), fileHighLevelRep(fileHighLevelRep)
{
    this->token.reserve(TOKEN_RESERVE_MIN);
}

void ParserBase::Transition(const State state, const int substateIndex)
{
    this->state = state;
    this->substateIndex = substateIndex;
}

void ParserBase::UpdateState(const char input)
{
    switch (state) {
        case State::CODE:
            switch (this->substateIndex) {
                case 0:
                    switch (input) {
                        case '/':
                            this->substateIndex++;
                            this->CloseRelation();
                            break;
                            
                        case '"':
                            this->Transition(State::STRING_LITERAL);
                            this->CloseRelation();
                            break;
                            
                        case '\'':
                            this->Transition(State::CHAR_LITERAL);
                            this->CloseRelation();
                            break;
                            
                        default:
                            // sea of tokens
                            break;
                    }
                    break;
                    
                case 1: // / was seen
                    switch (input) {
                        case '/':
                            this->Transition(State::COMMENT);
                            break;
                            
                        case '*':
                            this->Transition(State::COMMENT, 1);
                            break;
                            
                        default:
                            this->substateIndex = 0; // was div operator
                            break;
                    }
                    break;
                    
                default:
                    this->stateConsistency = false;
                    break;
            }
            
            break;
            
        case State::COMMENT:
            switch (this->substateIndex) {
                case 0:
                    switch (input) {
                        case '\r':
                        case '\n':
                        case '\0':
                            this->Transition(State::CODE);
                            break;
                            
                        default:
                            break;
                    }
                    break;
                    
                case 1:
                    switch (input) {
                        case '*':
                            this->substateIndex++;
                            break;
                            
                        default:
                            break;
                    }
                    break;
                    
                case 2:
                    switch (input) {
                        case '/':
                            this->Transition(State::CODE);
                            break;
                            
                        case '*':
                            break;
                            
                        default:
                            this->substateIndex--;
                            break;
                    }
                    break;
                    
                default:
                    this->stateConsistency = false;
                    break;
            }
            
            break;
            
        case State::CHAR_LITERAL:
            switch (this->substateIndex) {
                case 0:
                    switch (input) {
                        case '\'':
                            this->Transition(State::CODE);
                            break;
                            
                        case '\\':
                            this->substateIndex--;
                            break;
                            
                        default:
                            break;
                    }
                    break;
                    
                case -1: // escape
                    this->substateIndex = 0;
                    break;
                    
                default:
                    this->stateConsistency = false;
                    break;
            }
            break;
            
        case State::STRING_LITERAL:
            this->UpdateStateStringLiteral(input);
            break;
            
        case State::TEXT_BLOCK:
            this->UpdateStateTextBlock(input);
            break;
            
        default:
            this->stateConsistency = false;
            break;
    }
    
    // state machine post update
    // this does not modify the state or substate
    // this cannot be integrated in state machine update
    switch (this->state) {
        // update file high level representation depth
        case State::CODE:
            switch (this->substateIndex) {
                case 0:
                    switch (input) {
                        case '{':
                        case '(':
                        case '[':
                            this->fhlrDepth++;
                            this->fileHighLevelRep.SetMaxDepth(this->fhlrDepth);
                            this->CloseRelation();
                            break;
                            
                        case '}':
                        case ')':
                        case ']':
                            this->fhlrDepth--;
                            this->CloseRelation();
                            break;
                            
                        case ';':
                            this->CloseRelation();
                            break;

                        default:
                            // update token
                            if (std::isalnum(input) || '_' == input) {
                                this->token += input;
                            }
                            else {
                                this->CloseToken();
                            }
                            break;
                    }
                    break;
                    
                default:
                    break;
            }
            break;
            
        // update comment energy
        case State::COMMENT:
            if (std::isalnum(input)) {
                this->fileHighLevelRep.IncrementCommentEnergy();
            }
            break;
            
        default:
            break;
    }
}

void ParserBase::UpdateStateStringLiteral(const char input)
{
    switch (this->substateIndex) {
        case 0:
            switch (input) {
                case '\\': // escape
                    this->substateIndex = -1;
                    break;
                    
                case '"':
                    this->Transition(State::CODE);
                    // fall
                    
                default:
                    break;
            }
            break;
            
        case -1: // escape
            this->substateIndex = 0;
            break;

        default:
            break;
    }
}

void ParserBase::CloseToken()
{
    const size_t tokenLen = this->token.length();
    if (tokenLen > 0) {
        /*// check number literal
        if (std::isdigit(this->token[0])) {
            this->token = NUMBER_LITERAL;
        }*/
        
        if (Relation::POS_UNINIT == this->relation.pos) {
            this->relation.pos = static_cast<int>(this->lineStartPos);
            this->relation.lineCount++;
        }
        
        // push
        std::vector<std::string>& tl = std::get<std::vector<std::string>>(this->relation.tokenList);
        tl.push_back(this->token);
        this->token.clear();
    }
}

void ParserBase::CloseRelation()
{
    this->CloseToken();
    std::vector<std::string>& tl = std::get<std::vector<std::string>>(this->relation.tokenList);
    if (!tl.empty()) {
        this->fileHighLevelRep.CloseRelation(this->relation);
        this->relation.Reset();
        tl.clear();
    }
}

void ParserBase::IncrementLineCount()
{
    if (Relation::POS_UNINIT != this->relation.pos) {
        if (this->relation.lineCount < Relation::LINE_COUNT_MAX) {
            this->relation.lineCount++;
        }
    }
}

void ParserBase::SetLineStartPos(const long long pos)
{
    if (pos < Relation::POS_UNINIT) {
        this->lineStartPos = pos;
    }
    else {
        this->lineStartPos = Relation::POS_UNINIT - 1;
        std::cerr << "Bad pos:\t" << pos << std::endl;
    }
}
