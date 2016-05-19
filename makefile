OPT_OR_DEBUG=-o2
CFLAGS=-pedantic

all:server client

server:server.h local.h folderhandler.o
	cc $(CFLAGS) server.c -o server

client:client.h local.h folderhandler.o
	cc $(CFLAGS) client.c -o client

folderhandler.o:folderhandler.h
	cc $(CFLAGS) -c folderhandler.c

clean:
	rm -f server client  $(wildcard *.o)
