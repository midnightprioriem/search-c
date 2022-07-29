#ifndef SEARCH_H
#define SEARCH_H

#include <stdlib.h>

typedef struct SearchTrie SearchTrie;

typedef struct SearchResult SearchResult;
struct SearchResult
{
    char *result_string;
    SearchResult *next_result;
};

typedef enum MatchType MatchType;
enum MatchType
{
    match_type_exact,
    match_type_all
};

typedef struct SearchRequest SearchRequest;
struct SearchRequest
{
    char *query_string;
    MatchType match_type;
};

typedef struct Search Search;
struct Search
{
    SearchResult *result;
    int (*SearchQuery)(Search *this, SearchRequest* query);
    int (*FlushResults)(Search *this);
    void (*AddSearchResult)(Search *this, char *search_result);

    /* Private members */
    SearchTrie *_search_trie;
};

/* Constructors and Destructors */
Search *SearchInit();
void SearchDestroy(Search *search);

#endif