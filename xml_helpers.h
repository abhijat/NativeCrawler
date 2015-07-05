#ifndef XML_HELPERS_GUARD_
#define XML_HELPERS_GUARD_

#include <stdio.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/parserInternals.h>

#include "curl_helpers.h"

typedef struct
{
    char str[1024];
    int size;
} element_t;

xmlDocPtr getdoc(const char* filename);
xmlXPathObjectPtr getnodeset(xmlDocPtr doc, xmlChar* xpath);
void print_results(xmlDocPtr doc, xmlChar* query);
void callback(void* data, const xmlChar* name, const xmlChar** attrs);
void data_callback(void* data, const xmlChar* ch, int size);
void end_callback(void* data, const xmlChar* name);
void parse_xml(const string_t* str);
void query(const char* filename);

#endif
