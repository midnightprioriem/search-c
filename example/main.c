#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>

#define MAX_LINE 100

void remove_newline(char *string)
{
    string[strcspn(string, "\n")] = 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        puts("Usage: ./search-example words.txt");
    }
    const char *words_file_path = argv[1];

    Search *search = SearchInit();

    printf("Initializing search library with %s\n", words_file_path);

    FILE *words_file;
    char word_str[MAX_LINE] = {0};
    words_file = fopen(words_file_path, "r");
    if (words_file == NULL)
    {
        printf("Error opening words dictionary\n");
        return 1;
    }
    while (fgets(word_str, MAX_LINE, words_file) != NULL)
    {
        remove_newline(word_str);
        search->AddSearchResult(search, word_str);
    }
    fclose(words_file);

    while (1)
    {
        printf("Search:");
        char query_string[MAX_LINE] = {0};
        fgets(query_string, MAX_LINE, stdin);
        remove_newline(query_string);
        SearchRequest req = {query_string, match_type_all};
        if (search->SearchQuery(search, &req))
        {
            puts("Results:");
            SearchResult *result = search->result;
            while (result)
            {
                puts(result->result_string);
                result = result->next_result;
            }
        }
        else
        {
            printf("Search did not find a match for %s\n", query_string);
        }
    }

    SearchDestroy(search);
}