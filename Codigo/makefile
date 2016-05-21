OPT_OR_DEBUG=-o2
CFLAGS=-pedantic

all:sobusrv sobucli

folderhandler.o:folderhandler.h
	cc $(CFLAGS) -c folderhandler.c

sobusrv:server.c server.h local.h folderhandler.o
	cc $(CFLAGS) server.c folderhandler.c -o sobusrv

sobucli:client.c client.h local.h folderhandler.o
	cc $(CFLAGS) client.c folderhandler.o -o sobucli

clean:
	rm -f sobusrv sobucli  $(wildcard *.o)
