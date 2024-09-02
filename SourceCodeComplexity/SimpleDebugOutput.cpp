//
//  SimpleDebugOutput.cpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 21.08.24.
//

#include "SimpleDebugOutput.hpp"

SimpleDebugOutput::SimpleDebugOutput(const std::string& filename) : enabled(false)
{
    this->f.open(filename);
}

void SimpleDebugOutput::SetEnabled(const bool enabled)
{
    this->enabled = enabled;
}

void SimpleDebugOutput::Newline()
{
    if (this->enabled) {
        if (this->f.is_open()) {
            this->f << std::endl;
        }
    }
}

void SimpleDebugOutput::operator<<(const int value)
{
    if (this->enabled) {
        if (this->f.is_open()) {
            this->f << value;
        }
    }
}

void SimpleDebugOutput::operator<<(const ParserBase::State parseState)
{
    if (this->enabled) {
        if (this->f.is_open()) {
            this->f << static_cast<int>(parseState);
        }
    }
}
