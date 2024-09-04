//
//  ParserJava.cpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 25.08.24.
//

#include "ParserJava.hpp"

ParserJava::ParserJava(FileHighLevelRepresentation& fileHighLevelRep) : ParserBase(fileHighLevelRep)
{
}

void ParserJava::UpdateStateStringLiteral(const char input)
{
    switch (this->substateIndex) {
        case 0:
            switch (input) {
                case '\\': // escape
                    this->substateIndex = -1;
                    break;
                    
                case '"':
                    this->substateIndex++;
                    // fall
                    
                default:
                    this->substateIndex++;
                    break;
            }
            break;
            
        case 1:
            switch (input) {
                case '\\': // escape
                    this->substateIndex = -2;
                    break;

                case '"':
                    this->Transition(State::CODE);
                    break;
                    
                default:
                    break;
            }
            break;
            
        case 2:
            if ('"' == input) {
                this->Transition(State::TEXT_BLOCK);
            }
            else {
                this->Transition(State::CODE);
            }
            break;
            
        case -1: // escape
            this->substateIndex = 0;
            break;

        case -2: // escape
            this->substateIndex = 0;
            this->substateIndex++;
            break;

        default:
            break;
    }
}

void ParserJava::UpdateStateTextBlock(const char input)
{
    switch (this->substateIndex) {
        case -1: // escape
            this->substateIndex = 0;
            break;
            
        case 0:
            switch (input) {
                case '\\': // escape
                    this->substateIndex = -1;
                    break;
                    
                case '"':
                    this->substateIndex++;
                    break;
                    
                default:
                    break;
            }
            break;
            
        case 1:
            if ('"' == input) {
                this->substateIndex++;
            }
            else {
                this->substateIndex = 0;
            }
            break;
            
        case 2:
            if ('"' == input) {
                this->Transition(State::CODE);
            }
            else {
                this->substateIndex = 0;
            }
            break;
            
        default:
            break;
    }
}
