//
//  Parser.cpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 19.08.24.
//

#include <string>

#include "Parser.hpp"

static const size_t TOKEN_RESERVE_SIZE = 5;
static const std::string NUMBER_LITERAL = "__NUMBER_LITERAL";

ParserBase::ParserBase()
: state(State::CODE), substateIndex(0), stateConsistency(true), fileHighLevelRep(nullptr)
{
    this->token.reserve(TOKEN_RESERVE_SIZE);
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
                            this->fileHighLevelRep->SetMaxDepth(this->fhlrDepth);
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
                            // update token relation
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
                this->fileHighLevelRep->IncrementCommentEnergy();
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
        
        // push
        this->relation.tokenList.push_back(this->token);
        this->token.clear();
        this->token.reserve(std::max(TOKEN_RESERVE_SIZE, static_cast<size_t>(1.25f * tokenLen)));
    }
}

void ParserBase::CloseRelation()
{
    this->CloseToken();
    if (!this->relation.tokenList.empty()) {
        this->fileHighLevelRep->CloseRelation(this->relation);
        this->relation.tokenList.clear();
    }
}
