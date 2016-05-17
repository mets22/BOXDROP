//
// Created by mets on 16-05-2016.
//

#include "client.h"
#include "local.h"

int main()
{
    int publicfifo, privatefifo, n;
    static char buffer[PIPE_BUF];
    struct message msg;

/*Using sprintf to create a unique fifo name
and save into message structure*/
    sprintf(msg.fifo_name, "/home/%d/.Backup/", getpid());
    msg.pid = getpid();
/*Creating the PRIVATE fifo*/
    if(mkfifo(msg.fifo_name,0666) < 0) {
        perror(msg.fifo_name);
        exit(1);
    }

/*Opening PUBLIC fifo in WRITE ONLY mode*/
    if((publicfifo = open(PUBLIC,O_WRONLY)) < 0) {
        unlink(msg.fifo_name);
        perror(PUBLIC);
        exit(1);
    }

    while(1) {

        write(fileno(stdout), "\n cmd>", 6);
        memset(msg.cmd_line, 0x0, B_SIZE);
        n = read(fileno(stdin), msg.cmd_line, B_SIZE);

        if(strncmp("quit", msg.cmd_line, n-1) == 0) {
            break;
        }

        write(publicfifo, &msg, sizeof(msg));

        if((privatefifo = open(msg.fifo_name, O_RDONLY)) < 0) {
            printf("1\n");
            perror(msg.fifo_name);
            goto CLEANUP;
        }

        while((n = read(privatefifo, buffer, PIPE_BUF)) > 0) {
            write(fileno(stderr), buffer, n);
        }

        close(privatefifo);
    }

    CLEANUP:
    close(publicfifo);
    unlink(msg.fifo_name);

    return 0;
}