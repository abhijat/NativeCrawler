#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

const unsigned BUFSIZE = 512;

typedef struct {
    char* buf;
    int size;
} string_t;

void free_string(string_t* s)
{
    free(s->buf);
    s->buf = NULL;
}

size_t write_callback(void* input, size_t size, size_t nmemb, void* data)
{
    size_t data_size = size * nmemb;
    string_t* pstr = (string_t*) data;

    char* t = realloc(pstr->buf, pstr->size + data_size);
    if (t != NULL) {
        pstr->buf = t;
    } else {
        printf("realloc failed...\n");
        free_string(pstr);
        return 0;
    }

    memcpy(pstr->buf + pstr->size, input, data_size);
    pstr->size += data_size;

    pstr->buf[pstr->size] = 0;
    return data_size;
}

int main(int argc, char* argv[0])
{
    CURL* curl;
    curl = curl_easy_init();
    if (curl) {
        string_t s;
        s.buf = calloc(sizeof(char), 1);
        s.size = 0;

        if (argc == 2 && argv[1][0] == 'v')
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, "http://feeds.bbci.co.uk/news/world/rss.xml");

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        CURLcode res = curl_easy_perform(curl);
        if (res  != CURLE_OK)
            printf("error: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        printf("%s\n", s.buf);
    }
}
