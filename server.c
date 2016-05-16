//
// Created by mets on 29-04-2016.
//

#include "server.h"
#include "local.h"

int existeFicheiro()

int main() {

    int privatefifo, dummyfifo, publicfifo, n, done;
    struct message msg;
    FILE *fin;
    static char buffer[PIPE_BUF];
    char comando[10];

/*creating the PUBLIC fifo*/
    mkfifo(PUBLIC,0666);

/*
Server process opens the PUBLIC fifo in write mode to make sure that
the PUBLIC fifo is associated with atleast one WRITER process. As a
result it never receives EOF on the PUBLIC fifo. The server process
will block any empty PUBLIC fifo waiting for additional messages to
be written. This technique saves us from having to close and reopen
the public FIFO every time a client process finishes its activities.
*/

    if( (publicfifo = open(PUBLIC, O_RDONLY)) < 0 ||
        (dummyfifo = open(PUBLIC, O_WRONLY | O_NDELAY)) < 0) {
        perror(PUBLIC);
        exit(1);
    }

/*Read the message from PUBLIC fifo*/
    while(read(publicfifo, &msg, sizeof(msg)) > 0) {

        pid = fork();

        if(pid == 0) {

                if ((privatefifo = open(msg.fifo_name, O_WRONLY | O_NDELAY)) == -1) {
                    sleep(5);
                }
                else {
                    /*fin = popen(msg.cmd_line, "r");
                    write(privatefifo, "\n", 1);

                    while ((n = read(fileno(fin), buffer, PIPE_BUF)) > 0) {
                        write(privatefifo, buffer, n);
                        memset(buffer, 0x0, PIPE_BUF);
                    }

                    pclose(fin);
                    close(privatefifo);
                    done = 1;*/



                    switch(){
                        case "backup" : ;
                            break;
                        case "restore" : ;
                            break;
                    }
                }

            if (!done) {
                perror("Not accessed the private fifo\n");
                exit(1);
            }
        }

    }
    return 0;
}