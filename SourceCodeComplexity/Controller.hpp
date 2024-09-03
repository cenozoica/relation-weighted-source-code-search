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
    
    // a token to be searched that may not match exactly the search string
    struct SearchToken
    {
        unsigned int token;
        float weight;
    };
    std::vector<SearchToken> searchToken;
    std::vector<int> searchResultTop; // max 10 energy files as indexes
    static const size_t SEARCH_RESULT_TOP_SIZE_MAX = 10;
public:
    Controller(const std::vector<std::filesystem::path>& filelist);
    void Analyze(); // no concurrency
    void AnalyzeParallel(SimpleDebugOutput& simpleDebugOutput);
    void CreateUniqueTokenList();
    void Search(const std::string& q);

    // to file methods
    void ToFileComplexity(const std::string filename) const;
    void ToFileTokenSet(const std::string filename) const;
    void ToFileSearchResult(const std::string filename) const;
};

#endif /* Controller_hpp */
