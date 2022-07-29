#include <string.h>
#include <stdio.h>

#include "search.h"

/* Defines */
#define ALPHABET_SIZE (126 - '!')
#define TRIE_INDEX(c) (c - '!')

/* Search Trie Private Implementation */

struct SearchTrie
{
    int end_of_word;
    int depth;
    char c;
    SearchTrie *parent;
    SearchTrie *children[ALPHABET_SIZE];
};

/* Private Functions */
static void TrieFree(SearchTrie *trie);
static SearchTrie *NewTrieNode(SearchTrie *parent, char c);
static int TrieInsert(SearchTrie *trie, char *word);
static SearchTrie *TrieNavigate(SearchTrie *trie, char *word);
static void InsertResultStringFromTrieLeaf(Search *search, SearchTrie *leaf);
static int TrieSearch(SearchTrie *trie, char *word);
static void TrieSearchPartialRecurse(Search *search, SearchTrie *node, char *word);
static int TrieSearchPartial(Search *search, char *word);
static SearchResult *NewResult(char *result_string);
static void FreeResults(Search *search);

static void TrieFree(SearchTrie *trie)
{
    if (!trie)
    {
        return;
    }
    for (size_t i = 0; i < ALPHABET_SIZE; i++)
    {
        TrieFree(trie->children[i]);
    }
    free(trie);
}

static SearchTrie *NewTrieNode(SearchTrie *parent, char c)
{
    SearchTrie *new_node = (SearchTrie *)malloc(sizeof(SearchTrie));
    new_node->end_of_word = 0;
    for (size_t i = 0; i < ALPHABET_SIZE; i++)
    {
        new_node->children[i] = NULL;
    }
    new_node->parent = parent;
    if (parent)
    {
        new_node->depth = parent->depth + 1;
    }
    else
    {
        new_node->depth = 0;
    }
    new_node->c = c;
    return new_node;
}

static int TrieInsert(SearchTrie *trie, char *word)
{
    if (!trie)
    {
        return 0;
    }

    SearchTrie *current_node = trie;
    while (*word)
    {
        size_t i = TRIE_INDEX(*word);
        if (i > ALPHABET_SIZE - 1)
        {
            return 0; // TODO there should be handling here to remove nodes that were added
        }
        if (!current_node->children[i])
        {
            current_node->children[i] = NewTrieNode(current_node, *word);
        }
        current_node = current_node->children[i];
        word++;
    }
    current_node->end_of_word = 1;
    return 1;
}

static SearchTrie *TrieNavigate(SearchTrie *trie, char *word)
{
    SearchTrie *current_node = trie;
    while (current_node && *word)
    {
        size_t i = TRIE_INDEX(*word);
        if (!current_node->children[i])
        {
            return 0;
        }
        else
        {
            current_node = current_node->children[i];
            word++;
        }
    }
    return current_node;
}

static int TrieSearch(SearchTrie *trie, char *word)
{
    SearchTrie *end = TrieNavigate(trie, word);
    return end ? end->end_of_word : 0;
}

static void InsertResultStringFromTrieLeaf(Search *search, SearchTrie *leaf)
{
    int depth = leaf->depth;
    char *result_string = (char *)malloc(sizeof(char) * (depth + 1));
    result_string[depth] = 0;
    SearchTrie *node = leaf;
    while (depth > 0)
    {
        result_string[depth - 1] = node->c;
        depth--;
        node = node->parent;
    }

    if (search->result)
    {
        SearchResult *end_result = search->result;
        while (end_result->next_result)
        {
            end_result = end_result->next_result;
        }
        end_result->next_result = NewResult(result_string);
    }
    else
    {
        search->result = NewResult(result_string);
    }
    free(result_string);
}

static void TrieSearchPartialRecurse(Search *search, SearchTrie *node, char *word)
{
    if (node->end_of_word)
    {
        InsertResultStringFromTrieLeaf(search, node);
    }

    for (size_t i = 0; i < ALPHABET_SIZE; i++)
    {
        if (node->children[i])
        {
            TrieSearchPartialRecurse(search, node->children[i], word);
        }
    }
}

static int TrieSearchPartial(Search *search, char *word)
{
    SearchTrie *partial_node = TrieNavigate(search->_search_trie, word);
    if (partial_node)
    {
        TrieSearchPartialRecurse(search, partial_node, word);
    }
    return search->result != NULL;
}

static SearchResult *NewResult(char *result_string)
{
    SearchResult *new_result = malloc(sizeof(SearchResult));
    size_t len = strlen(result_string) + 1;
    new_result->result_string = malloc(sizeof(char) * len);
    strncpy(new_result->result_string, result_string, len);
    new_result->next_result = NULL;
    return new_result;
}

static void FreeResults(Search *search)
{
    /* Free results */
    SearchResult *result = search->result;
    while (result)
    {
        free(result->result_string);
        SearchResult *next_result = result->next_result;
        free(result);
        result = next_result;
    }
}

/* Member Variables for Search */
static int SearchQuery(Search *this, SearchRequest *query);
static void AddSearchResult(Search *this, char *search_result);
static int FlushResults(Search *this);

static int SearchQuery(Search *this, SearchRequest *query)
{
    FlushResults(this);
    int success = 0;
    switch (query->match_type)
    {
    case match_type_all:
        TrieSearchPartial(this, query->query_string);
        if (this->result)
        {
            success = 1;
        }
        break;
    case match_type_exact:
        if (TrieSearch(this->_search_trie, query->query_string))
        {
            this->result = NewResult(query->query_string);
            success = 1;
        }
        break;
    }
    return success;
}

static void AddSearchResult(Search *this, char *search_result)
{
    if (!this->_search_trie)
    {
        return;
    }
    TrieInsert(this->_search_trie, search_result);
}

static int FlushResults(Search *this)
{
    FreeResults(this);
    this->result = NULL;
}

Search *SearchInit()
{
    Search *search = (Search *)malloc(sizeof(Search));
    search->SearchQuery = SearchQuery;
    search->FlushResults = FlushResults;
    search->AddSearchResult = AddSearchResult;
    search->result = NULL;
    search->_search_trie = NewTrieNode(NULL, 0);

    return search;
}

void SearchDestroy(Search *search)
{
    /* Free Results */
    FreeResults(search);

    /* Free Trie */
    TrieFree(search->_search_trie);

    free(search);
}