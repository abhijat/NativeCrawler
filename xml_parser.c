#include "xml_parser.h"
#include <libxml/parserInternals.h>
#include <stdbool.h>

element_t* e = NULL;
element_list_node_t* root = NULL;

const char* filter_list[] = {
    "link", "title", "category", "pubDate", "item", "description"
};

typedef struct
{
    char* title;
    char* link;
    char* category;
    char* pubdate;
    char* description;
} reddit_post_t;
reddit_post_t* post = NULL;

const int filter_size = sizeof(filter_list) / sizeof(filter_list[0]);
bool processing = false;

bool filter_element(const xmlChar* name)
{
    for (int i = 0; i < filter_size; ++i) {
        if (!xmlStrcmp((const xmlChar*) filter_list[i], name)) 
            return true;
    }
    return false;
}

void callback(void* data, const xmlChar* name, const xmlChar** attrs)
{
    if (!filter_element(name)) {
        processing = false;
        return;
    } else {
        processing = true;
    }

    e = malloc(sizeof(element_t));
    strncpy(e->name, (const char*) name, 64);

    if (root == NULL) {
        root = malloc(sizeof(element_list_node_t));
        root->e = e;
        root->next = NULL;
    } else {
        element_list_node_t* node = malloc(sizeof(element_list_node_t));
        node->e = e;
        node->next = NULL;

        element_list_node_t* t = root;
        while (t->next != NULL) t = t->next;
        t->next = node;
    }

    memset(e->str, 0, 1024);
    e->size = 0;

    if (xmlStrcmp(name, (const xmlChar*) "item") == 0) {
        post = malloc(sizeof(*post));
    }
}

void data_callback(void* data, const xmlChar* ch, int size)
{
    if (!processing)
        return;

    // TODO use an expandable structure here instead of a fixed size
    if (e->size >= 1024) return;
    
    char buf[size + 1];
    strncpy(buf, (const char*)ch, size);
    buf[size] = 0;

    strncat(e->str, buf, size);
    e->size += size;
}

void print_post(const reddit_post_t* post)
{
    printf("%s \n"
            "/r/%s :: %s\n"
            "\t[%s]\n"
            "%s\n"
            "\n",
            post->title, 
            post->category, 
            post->pubdate,
            post->link,
            post->description);
}

void free_ptr(void** p)
{
    if (*p) {
        free(*p);
        *p = NULL;
    }
}

void free_post(reddit_post_t** post)
{
    free_ptr(&(*post)->title); 
    free_ptr(&(*post)->link); 
    free_ptr(&(*post)->category); 
    free_ptr(&(*post)->description); 
    free_ptr(&(*post)->pubdate); 
    free_ptr(post);
}

void end_callback(void* data, const xmlChar* name)
{
    if (!filter_element(name))
        return;
    processing = false;

    if (xmlStrcmp(name, (const xmlChar*) "item") == 0) {
        print_post(post);
        free_post(&post);
    }

    if (post != NULL) { 
        if (xmlStrcmp(name, (const xmlChar*) "title") == 0) {
            post->title = calloc(1, e->size + 1);
            strncpy(post->title, e->str, e->size);
        }

        if (xmlStrcmp(name, (const xmlChar*) "link") == 0) {
            post->link = calloc(1, e->size + 1);
            strncpy(post->link, e->str, e->size);
        }

        if (xmlStrcmp(name, (const xmlChar*) "category") == 0) {
            post->category = calloc(1, e->size + 1);
            strncpy(post->category, e->str, e->size);
        }

        if (xmlStrcmp(name, (const xmlChar*) "description") == 0) {
            post->description = calloc(1, e->size + 1);
            if (post->description == NULL) printf("DANGER\n");
            strncpy(post->description, e->str, e->size);
        }

        if (xmlStrcmp(name, (const xmlChar*) "pubDate") == 0) {
            post->pubdate = calloc(1, e->size + 1);
            strncpy(post->pubdate, e->str, e->size);
        }
    }
}

void free_elements()
{
    element_list_node_t* t = root;
    while (t != NULL) {
        element_list_node_t* tmp = t;
        t = t->next;
        free(tmp->e);
        free(tmp);
    }
}

void parse_xml(const string_t* str)
{
    xmlSAXHandler* sh = calloc(1, sizeof(*sh));

    sh->startElement = callback;
    sh->characters = data_callback;
    sh->endElement = end_callback;

    xmlParserCtxtPtr ctx = 
        xmlCreateMemoryParserCtxt(str->buf, str->size);

    ctx->sax = sh;
    xmlParseDocument(ctx);

    xmlFreeParserCtxt(ctx);

    /*element_list_node_t* t = root;
    while (t != NULL) {
        char* key = t->e->name;
        char* value = t->e->str;

        if (!strcmp(key, "title")) {
            printf("%s\n", value);
        }

        if (!strcmp(key, "category")) {
            printf("  %-20s /r/%s\n", "subreddit:", value);
        }

        if (!strcmp(key, "link")) {
            printf("  %-20s %s\n", "url:", value);
        }

        if (!strcmp(key, "pubDate")) {
            printf("  %-20s %s\n\n", "published:", value);
        }

        t = t->next;
    }*/

    free_elements();
    free(post);
}
