#include "curl_helpers.h"
#include "xml_helpers.h"

int main()
{    
    string_t str = read_url("http://feeds.bbci.co.uk/news/rss.xml");
    parse_xml(&str);
    free(str.buf);
    return 0;
}
