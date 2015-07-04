#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/parserInternals.h>

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

typedef struct
{
    char str[1024];
    int size;
} element_t;
element_t element;

void callback(void* data, const xmlChar* name, const xmlChar** attrs)
{
    /*printf("Beginning with %s\n", name);
    while (NULL != attrs && NULL != attrs[0]) {
        printf("attribute %s=%s\n", attrs[0], attrs[1]);
        attrs = &attrs[2];
    }*/

    memset(element.str, 0, 1024);
    element.size = 0;

}

void data_callback(void* data, const xmlChar* ch, int size)
{
    if (element.size >= 1024) return;
    
    char buf[size + 1];
    strncpy(buf, (const char*)ch, size);
    buf[size] = 0;
    
    //printf("\tData: %s\n", buf); 
    
    strncat(element.str, buf, size);
    element.size += size;
}

void end_callback(void* data, const xmlChar* name)
{
    if (!xmlStrcmp(name, (const xmlChar*) "title")
            || !xmlStrcmp(name, (const xmlChar*) "link")) {
        printf("%s: %s\n", name, element.str);
    }
}

void parse_xml(const char* filename)
{
    xmlSAXHandler sh = { 0 };

    sh.startElement = callback;
    sh.characters = data_callback;
    sh.endElement = end_callback;

    xmlParserCtxtPtr ctx = xmlCreateFileParserCtxt(filename);
    ctx->sax = &sh;
    xmlParseDocument(ctx);
}

void query(const char* filename)
{
    xmlDocPtr doc = getdoc("test_data");
    print_results(doc, (xmlChar*) "/rss/channel/item/link");
}

void fetch_url()
{
    CURL* curl;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, 
                "http://www.reddit.com/r/all.xml");

        FILE* fh = fopen("test_data", "w");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fh);

        printf("Fetching URL to tmp file...\n");
        fflush(stdout);

        CURLcode rc = curl_easy_perform(curl);
        if (rc != CURLE_OK) {
            printf("curl_easy_perform() failed with error %s\n",
                    curl_easy_strerror(rc));
        }

        curl_easy_cleanup(curl);
        fclose(fh);
    }
}

int main()
{    
    fetch_url();
    parse_xml("test_data");
    return 0;
}
