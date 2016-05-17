OPT_OR_DEBUG=-o2
CFLAGS=-pedantic

all:server client

server:server.h local.h
	cc $(CFLAGS) server.c -o server

client:client.h local.h
	cc $(CFLAGS) client.c -o client

clean:
	rm -f server client  $(wildcard *.o)
