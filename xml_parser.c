#include "xml_parser.h"
#include <libxml/parserInternals.h>
#include <stdbool.h>

element_t* e = NULL;
element_list_node_t* root = NULL;

const char* filter_list[] = {
    "link", "title", "category", "pubDate"
};

const int filter_size = sizeof(filter_list) / sizeof(filter_list[0]);
bool processing = false;

bool filter_element_by_name(const xmlChar* name)
{
    for (int i = 0; i < filter_size; ++i) {
        if (!xmlStrcmp((const xmlChar*) filter_list[i], name)) 
            return true;
    }
    return false;
}

void callback(void* data, const xmlChar* name, const xmlChar** attrs)
{
    /*
    printf("Beginning with %s\n", name);
    while (NULL != attrs && NULL != attrs[0]) {
        printf("attribute %s=%s\n", attrs[0], attrs[1]);
        attrs = &attrs[2];
    }
    */

    if (!filter_element_by_name(name)) {
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

void end_callback(void* data, const xmlChar* name)
{
    if (!filter_element_by_name(name))
        return;

    /*
    if (!xmlStrcmp(name, (const xmlChar*) "title")) {
        printf("===================\n"
                "%s\n", e->str);
    }

    if (!xmlStrcmp(name, (const xmlChar*) "description")) {
        printf("%s\n", e->str);
    }

    if (!xmlStrcmp(name, (const xmlChar*) "link")) {
        printf("(%s)"
                "\n===================\n\n", e->str);
    }
    */

    processing = false;
}

void parse_xml(const string_t* str)
{
    xmlSAXHandler sh = { 0 };

    sh.startElement = callback;
    sh.characters = data_callback;
    sh.endElement = end_callback;

    xmlParserCtxtPtr ctx = 
        xmlCreateMemoryParserCtxt(str->buf, str->size);
    ctx->sax = &sh;
    xmlParseDocument(ctx);

    element_list_node_t* t = root;
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
    }
}
