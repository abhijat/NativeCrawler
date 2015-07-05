#include "curl_helpers.h"

const unsigned BUFSIZE = 512;
const unsigned MAX_URL = 256;

void free_string(string_t* s)
{
    free(s->buf);
    s->buf = NULL;
}

size_t write_callback(void* input, size_t size, size_t nmemb, void* data)
{
    size_t data_size = size * nmemb;
    string_t* pstr = (string_t*) data;

    int need_size = data_size + pstr->size + 1;
    if (need_size > pstr->capacity) {
        while (pstr->capacity < need_size) {
            pstr->capacity += BUFSIZE;
        }
        char* t = realloc(pstr->buf, pstr->capacity);
        if (t != NULL) {
            pstr->buf = t;
        } else {
            printf("realloc failed...\n");
            free_string(pstr);
            return 0;
        }
    }

    memcpy(pstr->buf + pstr->size, input, data_size);

    pstr->size += data_size;
    pstr->buf[pstr->size] = 0;
    
    return data_size;
}

string_t read_url(const char* url)
{
    string_t err_str = {
        .buf = NULL,
        .size = -1,
        .capacity = -1
    };
    CURL* curl;
    curl = curl_easy_init();

    if (!curl) 
        return err_str;

    string_t s;
    s.buf = calloc(sizeof(char), BUFSIZE);
    s.size = 0;
    s.capacity = BUFSIZE;

    curl_easy_setopt(curl, CURLOPT_URL, url);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    CURLcode res = curl_easy_perform(curl);
    if (res  != CURLE_OK)
        printf("error: %s\n", curl_easy_strerror(res));

    string_t result = s;
    result.buf = strndup(s.buf, s.size);
    curl_easy_cleanup(curl);

    return result;
}

/*int main(int argc, char* argv[])
{

    char target[MAX_URL];

    if (argc == 2 && argv[1][0] == 'h') {
        strncpy(target, argv[1], MAX_URL);
    } else  {
        strncpy(target, "http://www.reddit.com/r/all.xml", MAX_URL);
    }
    char* content = read_url(target);
    if (content != NULL) printf("%s\n", content);
    free(content);
}*/
