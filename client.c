#include "client.h"

int main(int argc, char const *argv[]) {

    int publicfifo, privatefifo, i;
    static char buffer[PIPE_BUF];
    char *string = (char*)calloc(B_SIZE,sizeof(char));
    char *backuppath = getPathOfBackupFolder();
    char fifo[128];
    struct message msg;

    strcpy(fifo,backuppath);
    strcat(fifo,PUBLIC);
    /*Using sprintf to create a unique fifo name
    and save into message structure*/
    sprintf(msg.fifo_name, "%sfifo%d",backuppath, getpid());
    msg.pid = getpid();

    /*Creating the PRIVATE fifo*/
    if(mkfifo(msg.fifo_name,0666) < 0) {
        perror(msg.fifo_name);
        exit(1);
    }

    /*Opening PUBLIC fifo in WRITE ONLY mode*/
    if((publicfifo = open(fifo,O_WRONLY)) < 0) {
        unlink(msg.fifo_name);
        perror(PUBLIC);
        exit(1);
    }

    // Junta todos os argumentos numa so string

    for(i=1; i<argc-1; i++){
        strcat(string,argv[i]);
        strcat(string," ");
    }
    strcat(string,argv[i]);
    strcpy(msg.cmd_line,string);

    // escreve para o servidor
    write(publicfifo,&msg,sizeof(msg));
    if(privatefifo = open(msg.fifo_name,O_RDONLY)){
        unlink(msg.fifo_name);
        perror(PUBLIC);
        exit(1);
    }
    pause();
    free(string);
    unlink(msg.fifo_name);
    close(privatefifo);
    close(publicfifo);
    return 0;

}
