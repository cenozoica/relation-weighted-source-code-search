//
//  SimpleDebugOutput.hpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 21.08.24.
//

#ifndef SimpleDebugOutput_hpp
#define SimpleDebugOutput_hpp

#include <stdio.h>
#include <fstream>

#include "Parser.hpp"

class SimpleDebugOutput {
private:
    std::ofstream f;
    bool enabled;
public:
    SimpleDebugOutput(const std::string& filename);
    void SetEnabled(const bool enabled);
    void operator<<(const int value);
    void operator<<(const ParserBase::State parseState);
    void Newline();
};

#endif /* SimpleDebugOutput_hpp */
