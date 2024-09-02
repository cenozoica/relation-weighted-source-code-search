//
//  Controller.hpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 28.08.24.
//

#ifndef Controller_hpp
#define Controller_hpp

#include <memory>

#include "SimpleDebugOutput.hpp"
#include "FileDescriptor.hpp"

class Controller
{
private:
    std::vector<FileDescriptor> fileDescriptorList;
    std::unique_ptr<const std::vector<std::string>> tokenList; // sorted
    
    // search data
    struct SearchToken
    {
        unsigned int token;
        float weight;
    };
    std::vector<SearchToken> searchToken;
public:
    Controller(const std::vector<std::filesystem::path>& filelist);
    void Analyze(); // no concurrency
    void AnalyzeParallel(SimpleDebugOutput& simpleDebugOutput);
    void CreateUniqueTokenList();
    void ToFileComplexity(const std::string filename) const;
    void ToFileTokenSet(const std::string filename) const;
    void Search(const std::string& q);
};

#endif /* Controller_hpp */
