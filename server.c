//
// Created by mets on 29-04-2016.
//

#include "server.h"
#include "local.h"

int existeFicheiro(char *ficheiro)
{

}

void backupficheiro(char *ficheiro)
{
    char aux[128];
    c=strtok(ficheiro,".");
    strcpy(aux,ficheiro);
    execlp("cp","cp", aux, ,NULL);
}

void restoreficheiro(char *ficheiro)
{
    execlp("cp","cp", ,ficheiro, NULL);
}

int main()
{

    int privatefifo, dummyfifo, publicfifo, n, fd[2], pid,status;
    struct message msg;
    FILE *fin;
    static char buffer[PIPE_BUF];
    char comando[10], ficheiro[128], *c, metadata[161];
    const char *metadata =
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
        n++;
        if(n>5){
            waitpid(0,&status,0);
            n--;
        }
        pipe(fd);
        pid = fork();
        c=strtok(msg.cmd_line," ");
        strcpy(comando,msg.cmd_line);

        if(pid == 0) {
            c=strtok(NULL, " ");
            strcpy(ficheiro,msg.cmd_line);

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

                dup2(fd[1],1);
                close(fd[0]);
                close(fd[1]);
                execlp("gzip","gzip",ficheiro,NULL);
                strcat(ficheiro, ".gz");
                execlp("sha1sum","sha1sum", ficheiro, NULL);
                exit(3);
            }
        }
        else {
            close(fd[1]);
            read(fd[0],&metadata,160);
            while(read(fd[0],&c,1)>0) ficheiro++=*c;
            switch (comando) {
                case "backup" :;
                    break;
                case "restore" :;
                    break;
            }
        }

    }
    return 0;
}