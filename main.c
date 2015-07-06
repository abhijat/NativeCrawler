#include "curl_helpers.h"
#include "xml_parser.h"

int main()
{    
    string_t str = read_url("http://www.reddit.com/r/all.xml");
    parse_xml(&str);
    free(str.buf);
    str.buf = NULL;
    return 0;
}
