#include "client.h"
#include "local.h"

void main(int argc, char const *argv[]) {

    int publicfifo, privatefifo, i;
    static char buffer[PIPE_BUF];
    char * string; // o que vai ser envidado para o servidor
    struct message msg;

/*Using sprintf to create a unique fifo name
and save into message structure*/
    sprintf(msg.fifo_name, "/home/%d/.Backup/", getpid());

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

    // Junta todos os argumentos numa so string

    for(i=0; i<argc; i++){
        strcat(string,argv[i]);
        strcat(string," ");
    }

    // escreve para o servidor

    write(publicfifo,string,sizeof(string));

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

        close(privatefifo);
    //}
    close(publicfifo);

    /*CLEANUP:
    close(publicfifo);
    unlink(msg.fifo_name);*/

}
