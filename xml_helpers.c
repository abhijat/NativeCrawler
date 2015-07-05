#include "xml_helpers.h"

xmlDocPtr
getdoc(const char* filename)
{
    xmlDocPtr doc = xmlParseFile(filename);
    if (doc == NULL) {
        printf("parse error on doc %s\n", filename);
        return NULL;
    }
    return doc;
}

xmlXPathObjectPtr
getnodeset(xmlDocPtr doc, xmlChar* xpath)
{
    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
    return result;
}

void print_results(xmlDocPtr doc, xmlChar* query)
{
    xmlXPathObjectPtr result = getnodeset(doc, query);

    xmlChar* key;
    if (result) {
        xmlNodeSetPtr nodeset = result->nodesetval;
        for (int i = 0; i < nodeset->nodeNr; ++i) {
            key = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
            printf("%s\n", key);
            xmlFree(key);
        }
        xmlXPathFreeObject(result);
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
}

element_t element;
void callback(void* data, const xmlChar* name, const xmlChar** attrs)
{
    /*
    printf("Beginning with %s\n", name);
    while (NULL != attrs && NULL != attrs[0]) {
        printf("attribute %s=%s\n", attrs[0], attrs[1]);
        attrs = &attrs[2];
    }
    */

    memset(element.str, 0, 1024);
    element.size = 0;

}

void data_callback(void* data, const xmlChar* ch, int size)
{
    // TODO use an expandable structure here instead of a fixed size
    if (element.size >= 1024) return;
    
    char buf[size + 1];
    strncpy(buf, (const char*)ch, size);
    buf[size] = 0;

    strncat(element.str, buf, size);
    element.size += size;
}

void end_callback(void* data, const xmlChar* name)
{
    if (!xmlStrcmp(name, (const xmlChar*) "title")) {
        printf("===================\n"
                "%s\n", element.str);
    }

    if (!xmlStrcmp(name, (const xmlChar*) "description")) {
        printf("%s\n", element.str);
    }

    if (!xmlStrcmp(name, (const xmlChar*) "link")) {
        printf("(%s)"
                "\n===================\n\n", element.str);
    }
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
}

void query(const char* filename)
{
    xmlDocPtr doc = getdoc("test_data");
    print_results(doc, (xmlChar*) "/rss/channel/item/link");
}
