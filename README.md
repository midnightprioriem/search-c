# Search C

Just a small program written in C to test out a trie-based search algorithm.

## Build with CMake

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

```C
#include <search.h>

Search* search = Search_Init();

search->AddSearchResult(search, "delegate");
search->AddSearchResult(search, "delineate");
search->AddSearchResult(search, "demonstrate");

SearchRequest req = {"del", match_type_all};
if (search->SearchQuery(search, req)) {
    while(result) {
        puts(result->result_string);
        result = result->next_result;
    }
    /* Prints:
    *  delegate
    *  delineate
    */
}
```