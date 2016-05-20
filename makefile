OPT_OR_DEBUG=-o2
CFLAGS=-pedantic

all:server client

folderhandler.o:folderhandler.h
	cc $(CFLAGS) -c folderhandler.c

server:server.c server.h local.h folderhandler.o
	cc $(CFLAGS) server.c folderhandler.c -o server

client:client.c client.h local.h folderhandler.o
	cc $(CFLAGS) client.c folderhandler.o -o client

clean:
	rm -f server client  $(wildcard *.o)
