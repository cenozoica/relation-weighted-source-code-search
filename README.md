This project searches source code by building a high level represantation of the files, extracting relations between tokens, which can be number literals, identifiers, langauge keywords, and calculating the search result relevance from these relations.

The difference from classical IDE source code search is: do not return 0 or a very long list of files, sorted in alphabetical order, which is not relevant for software engineering, but rather find relations between code identifiers and sort the results using a very large matrix of relations (invisible to the user).

At this moment, the relations are not encoded, not approximated, not mapped in a n-dimensional space. The full information matrix is used during search. The approximation takes place during parsing, where the types of relations (resolution operator, arithmetic operator, etc) are abstracted.

The tokens and relations information provides a very simple means to deduce the complexity of the project, and in which files it is found.

For C/C++ code relations from map files (compiler/linker information) can be extracted too.

Processing intensive operations are parallel.

Comments are ignored.

Close projects are:
- Elasticsearch
- BERT (language model) by Google

Search result energy example:
![Search result energy example](https://github.com/cenozoica/relation-weighted-source-code-search/blob/main/search_result_energy_example.png?raw=true)
