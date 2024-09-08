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
    std::vector<unsigned int> searchResult;
    float searchResultEnergy;

    std::unique_ptr<ParserBase> CreateParser();
    bool IsFileExtensionAllowed() const; // not used now
public:
    static const size_t FILE_SIZE_MAX = 1U << 17;
    FileDescriptor(const std::filesystem::path& path);
    const std::filesystem::path& GetPath() const { return this->path; }
    void Analyze();
    bool IsAnalyzed() const { return this->analyzed; }
    bool HasHighLevelRepresentation() const { return nullptr != this->fileHighLevelRep; }
    void SetAnalyzed() { this->analyzed = true; }
    void ResetAnalyzed() { this->analyzed = false; }
    void ToGlobalTokenSet(std::set<std::string>& tokenSet);
    void Index(const std::vector<std::string>& tokenListGlobal);

    // complexity getters
    std::tuple<int, int, int> GetComplexity() const;
    
    // search methods
    void ResetSearchResult() { this->searchResult.clear(); this->searchResultEnergy = 0.0f; }
    void Search(const unsigned int q, const float weight = 1.0f);
    const std::vector<unsigned int>& GetSearchResult() const { return this->searchResult; }
    const Relation& GetRelation(const unsigned int i) const { return this->fileHighLevelRep->GetRelation(i); }
    float GetSearchResultEnergy() const { return this->searchResultEnergy; }
};

#endif /* FileDescriptor_hpp */
