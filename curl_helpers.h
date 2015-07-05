#ifndef CURL_HELPERS_GUARD_
#define CURL_HELPERS_GUARD_

#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

extern const unsigned BUFSIZE;
extern const unsigned MAX_URL;

typedef struct {
    char* buf;
    int size;
    int capacity;
} string_t;

void free_string(string_t* s);
size_t write_callback(void* input, size_t size, size_t nmemb, void* data);
string_t read_url(const char* url);

#endif
