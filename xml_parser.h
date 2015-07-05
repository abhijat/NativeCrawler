#ifndef XML_HELPERS_GUARD_
#define XML_HELPERS_GUARD_

#include <libxml/tree.h>

#include "curl_helpers.h"

typedef struct
{
    char name[64];
    char str[1024];
    int size;
} element_t;

struct element_list_node
{
    element_t* e;
    struct element_list_node* next;
};

typedef struct element_list_node element_list_node_t;

void callback(void* data, const xmlChar* name, const xmlChar** attrs);
void data_callback(void* data, const xmlChar* ch, int size);
void end_callback(void* data, const xmlChar* name);
void parse_xml(const string_t* str);

#endif
