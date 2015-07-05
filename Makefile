CC = gcc
CFLAGS = -g -Wall
LDFLAGS = $(shell curl-config --libs) $(shell pkg-config --cflags libxml-2.0) $(shell pkg-config --libs libxml-2.0)

main: main.c curl_helpers.c xml_helpers.c
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^
