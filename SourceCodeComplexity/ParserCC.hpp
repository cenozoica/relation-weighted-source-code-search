//
//  ParserCC.hpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 25.08.24.
//

#ifndef ParserCC_hpp
#define ParserCC_hpp

#include <stdio.h>
#include "Parser.hpp"

class ParserCC : public ParserBase
{
protected:
    void UpdateStateTextBlock(const char input) override {};
public:
    ParserCC(FileHighLevelRepresentation& fileHighLevelRep) : ParserBase(fileHighLevelRep) {}
};

#endif /* ParserCC_hpp */
