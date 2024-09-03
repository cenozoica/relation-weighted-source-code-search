//
//  main.cpp
//  SourceCodeComplexity
//
//  Created by Vadim Baitoiu on 18.08.24.
//

#include <iostream>
#include <filesystem>

#include "SimpleDebugOutput.hpp"
#include "Controller.hpp"

int main(int argc, const char * argv[]) {
    //FileHighLevelRepresentation::Test();
    SimpleDebugOutput simpleDebugOutput = SimpleDebugOutput("/Users/vadimbaitoiu/Downloads/sccdebug.log");
    std::vector<std::filesystem::path> filelist;

    // iterate source code directory
    std::string myPath = "/Users/vadimbaitoiu/Downloads/elasticsearch-main";
    using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
    for (const auto& dirEntry : recursive_directory_iterator(myPath)) {
        filelist.push_back(dirEntry.path());
    }

    // debug
    const std::filesystem::path p("/Users/vadimbaitoiu/Downloads/elasticsearch-main/x-pack/plugin/old-lucene-versions/src/main/java/org/elasticsearch/xpack/lucene/bwc/codecs/lucene60/Lucene60MetadataOnlyPointsFormat.java");
    ///fileComplexityVect.push_back(std::make_unique<FileComplexity>(p));
    ///std::system("open /Users/vadimbaitoiu/Downloads/elasticsearch-main/x-pack/plugin/old-lucene-versions/src/main/java/org/elasticsearch/xpack/lucene/bwc/codecs/lucene60/Lucene60MetadataOnlyPointsFormat.java");
    
    Controller controller(filelist);
    controller.AnalyzeParallel(simpleDebugOutput);
    controller.CreateUniqueTokenList();
    controller.Search("thread");

    controller.ToFileComplexity("/Users/vadimbaitoiu/Downloads/complexitydata.csv");
    //controller.ToFileTokenSet("/Users/vadimbaitoiu/Downloads/tokenset.txt");
    controller.ToFileSearchResult("/Users/vadimbaitoiu/Downloads/searchresult.txt");

    return 0;
}
