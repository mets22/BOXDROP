#include "client.h"

char *response = NULL;

void handler1(){
    response = strdup("copiado");
}
void handler2(){
    response = strdup("restaurado");
}

int main(int argc, char const *argv[]) {

    int publicfifo, i;
    static char buffer[PIPE_BUF];
    char *string = (char*)calloc(B_SIZE,sizeof(char));
    char *backuppath = getPathOfBackupFolder();
    char fifo[128];
    struct message msg;
    signal(SIGUSR1,handler1);
    signal(SIGUSR2,handler2);

    strcpy(fifo,backuppath);
    strcat(fifo,PUBLIC);
    /*Using sprintf to create a unique fifo name
    and save into message structure*/
    /*sprintf(msg.fifo_name, "%sfifo%d",backuppath, getpid());*/
    msg.pid = getpid();


    /*Opening PUBLIC fifo in WRITE ONLY mode*/
    if((publicfifo = open(fifo,O_WRONLY)) < 0) {
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
    pause();
    printf("%s : %s \n",argv[2],response);
    free(string);
    close(publicfifo);
    return 0;

}
