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
    void Analyze();
    bool IsAnalyzed() const { return this->analyzed; }
    void SetAnalyzed() { this->analyzed = true; }
    void ResetAnalyzed() { this->analyzed = false; }
    void ToGlobalTokenSet(std::set<std::string>& tokenSet);
    void UpdateIndex(const std::vector<std::string>& tokenListGlobal);

    // complexity getters
    int GetLineCount() const { return (nullptr == this-> fileHighLevelRep) ? 0 : this->fileHighLevelRep->GetLineCount(); }
    int GetMaxDepth() const { return (nullptr == this-> fileHighLevelRep) ? 0 : this->fileHighLevelRep->GetMaxDepth(); }
    int GetCommentEnergy() const { return (nullptr == this-> fileHighLevelRep) ? 0 : this->fileHighLevelRep->GetCommentEnergy(); }
    float GetSearchResultEnergy() const { return this->searchResultEnergy; }
    size_t GetRelationCount() const { return (nullptr == this-> fileHighLevelRep) ? 0 : this->fileHighLevelRep->GetRelationCount(); }
    size_t GetRelationEnergy() const { return (nullptr == this-> fileHighLevelRep) ? 0 : this->fileHighLevelRep->GetRelationEnergy(); }
    
    // search methods
    void ResetSearchResult() { this->searchResult.clear(); this->searchResultEnergy = 0.0f; }
    void Search(const unsigned int q, const float weight = 1.0f);
    std::vector<RelationCompressed>& GetSerchResult() { return this->searchResult; }
};

#endif /* FileDescriptor_hpp */
