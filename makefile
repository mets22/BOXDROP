OPT_OR_DEBUG=-o2
CFLAGS=-pedantic

all:server client

folderhandler.o:folderhandler.h
	cc $(CFLAGS) -c folderhandler.c

server:server.h local.h folderhandler.o
	cc $(CFLAGS) server.c folderhandler.c -o server

client:client.h local.h folderhandler.o
	cc $(CFLAGS) client.c -o client

clean:
	rm -f server client  $(wildcard *.o)
