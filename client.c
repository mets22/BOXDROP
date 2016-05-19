#include "client.h"
#include "local.h"

int main(int argc, char const *argv[]) {

    int publicfifo, privatefifo, i;
    static char buffer[PIPE_BUF];
    char *string = (char*)calloc(B_SIZE,sizeof(char));
    char fifo[128], user[64];
    struct message msg;

    strcpy(user,getenv("HOME"));
    strcpy(fifo,user);
    strcat(fifo,"/.Backup/");
    strcat(fifo,PUBLIC);
/*Using sprintf to create a unique fifo name
and save into message structure*/
    sprintf(msg.fifo_name, "/home/mets/.Backup/fifo%d", getpid());
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
    /*while(1) {

        if(argc>=3){
        if(strncmp("backup", argv[1], argv[1]) == 0)
            write(publicfifo,argv[1],sizeof(argv[1]));
              else {
                if(strncmp("restore", argv[1], argv[1]) == 0)
                  write(publicfifo,argv[1],sizeof(argv[1]));
                    else  write(2,"Insira um comando válido",30);}}
          else write(2,"Insira um comando válido",30);


        write(publicfifo, &msg, sizeof(msg));

        if((privatefifo = open(msg.fifo_name, O_RDONLY)) < 0) {
            printf("1\n");
            perror(msg.fifo_name);
            goto CLEANUP;
        }

        while((n = read(privatefifo, buffer, PIPE_BUF)) > 0) {
            write(fileno(stderr), buffer, n);
        }*/
        free(string);
        unlink(msg.fifo_name);
        close(privatefifo);
    //}
    close(publicfifo);
    return 0;
    /*CLEANUP:
    close(publicfifo);
    unlink(msg.fifo_name);*/

}
