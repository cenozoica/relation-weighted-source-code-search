//
//  ParserJava.hpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 25.08.24.
//

#ifndef ParserJava_hpp
#define ParserJava_hpp

#include <stdio.h>
#include "Parser.hpp"

class ParserJava : public ParserBase
{
private:
protected:
    void UpdateStateStringLiteral(const char input) override;
    void UpdateStateTextBlock(const char input) override;
public:
    ParserJava(FileHighLevelRepresentation& fileHighLevelRep);
};

#endif /* ParserJava_hpp */
