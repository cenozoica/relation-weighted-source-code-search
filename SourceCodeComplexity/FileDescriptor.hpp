//
//  FileDescriptor.hpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 21.08.24.
//

#ifndef FileDescriptor_hpp
#define FileDescriptor_hpp

#include <stdio.h>
#include <filesystem>

#include "Parser.hpp"

class FileDescriptor {
private:
    const std::filesystem::path& path;
    bool analyzed;
    std::unique_ptr<FileHighLevelRepresentation> fileHighLevelRep;
    std::vector<RelationCompressed> searchResult;
    float searchResultEnergy;

    std::unique_ptr<ParserBase> CreateParser();
    bool IsFileExtensionAllowed() const; // not used now
public:
    FileDescriptor(const std::filesystem::path& path);
    const std::filesystem::path& GetPath() const { return this->path; }
    void Analyze();
    bool IsAnalyzed() const { return this->analyzed; }
    void SetAnalyzed() { this->analyzed = true; }
    void ResetAnalyzed() { this->analyzed = false; }
    void ToGlobalTokenSet(std::set<std::string>& tokenSet);
    void UpdateIndex(const std::vector<std::string>& tokenListGlobal);

    // complexity getters
    std::tuple<int, int, int> GetComplexity() const;
    
    // search methods
    void ResetSearchResult() { this->searchResult.clear(); this->searchResultEnergy = 0.0f; }
    void Search(const unsigned int q, const float weight = 1.0f);
    const std::vector<RelationCompressed>& GetSearchResult() const { return this->searchResult; }
    float GetSearchResultEnergy() const { return this->searchResultEnergy; }
};

#endif /* FileDescriptor_hpp */
