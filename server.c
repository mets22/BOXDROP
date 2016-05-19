//
// Created by mets on 29-04-2016.
//

#include "server.h"
#include "local.h"

const char *metadata = "/.Backup/metadata";
const char *data = "/.Backup/data";
char user[64];

int existeFicheiro(char *ficheiro)
{
    int fd[2],fd2[2],x,temp,i,tlinha;
    char* slink,templinha,linha;
    pipe(fd);

    /*execlp("find","find","home/user/.Backup/metadata",ficheiro,0);
    while(fgets(linha,20,0)!=NULL){
          if(strcmp(linha,ficheiro)==0) return 1;
    }*/
    // escrever para 0 ler de 1

    if(x=fork()==0){
      close(fd[0]);
      while(read(fd[1],slink,20)!=0){
        readlink(slink,templinha,tlinha);
        linha=strndup(templinha,tlinha);
        printf("%s\n",linha);
        if(strcmp(linha,ficheiro)==0) return 1;
      }
      return 0;
    }else{
        close(fd[1]);
        dup2(1,fd[0]);
        execlp("find","find","-L","/home/goncalo/Documentos","-xtype","l","-printf","%f",0);

    }
}


void backupficheiro(char *ficheiro, char *shaisum, int pid)
{
    int status;
    char aux[128], newfile[512],location[128],linklocation[128],*c;

    strcpy(newfile, user);
    strcat(newfile, data);
    strcat(newfile, shaisum);
    strcpy(linklocation, metadata);
    c = strtok(ficheiro, ".");
    strcpy(aux, ficheiro);
    strcat(linklocation, aux);

    if(fork()==0) {
        execlp("cp", "cp", ficheiro, newfile, NULL);
    }
    else if(fork()==0) {
        execlp("touch", "touch", linklocation, NULL);
    }
    else if(fork()==0) {
        execlp("ln", "ln", newfile, linklocation, NULL);
    }
    else if(fork()==0) {
        execlp("rm", "rm", ficheiro, NULL);
    }
    else{
        waitpid(-1,&status,0);
        kill(pid,SIGCONT);
    }
}

/*void restoreficheiro(char *ficheiro)
{

    execlp("cp","cp", ,ficheiro, NULL);
}*/

int main()
{

    int privatefifo, dummyfifo, publicfifo, n, fd[2], pid,status,i=0;
    struct message msg;
    FILE *fin;
    static char buffer[PIPE_BUF];
    char comando[10], ficheiro[128], *c, shaisum[161], fifo[128];
    printf("AOK!\n");
    strcpy(user,getenv("HOME"));
    strcpy(fifo,user);
    strcat(fifo,"/.Backup/");
    mkdir(fifo,0777);
    
    strcat(fifo,PUBLIC);
/*creating the PUBLIC fifo*/
    printf("%d\n",mkfifo(fifo,0666));

/*
Server process opens the PUBLIC fifo in write mode to make sure that
the PUBLIC fifo is associated with atleast one WRITER process. As a
result it never receives EOF on the PUBLIC fifo. The server process
will block any empty PUBLIC fifo waiting for additional messages to
be written. This technique saves us from having to close and reopen
the public FIFO every time a client process finishes its activities.
*/
    printf("MORETHANOK!\n");
    if( (publicfifo = open(fifo, O_RDONLY)) < 0 ||
        (dummyfifo = open(fifo, O_WRONLY | O_NDELAY)) < 0) {
        perror(fifo);
        exit(1);
    }
    printf("lol\n");
/*Read the message from PUBLIC fifo*/
    while(read(publicfifo, &msg, sizeof(msg)) > 0) {
        n++;
        if(n>5){
            waitpid(0,&status,0);
            n--;
        }
        pipe(fd);
        c=strtok(msg.cmd_line," ");
        strcpy(comando,msg.cmd_line);
        printf("comando: %s\n",comando);
        printf("ciclo\n");
        if((pid = fork()) == 0) {
            c=strtok(NULL, " ");
            strcpy(ficheiro,c);
            if (privatefifo = open(msg.fifo_name, O_WRONLY | O_NDELAY) == -1) {
                printf("%d\n", privatefifo);
                perror(msg.fifo_name);
                exit(1);
            }
            else {
                printf("else\n");

                dup2(fd[1],1);
                close(fd[0]);
                printf("gzip\n");
                if(fork()==0) execlp("gzip","gzip",ficheiro,NULL);
                else {
                    waitpid(0,&status,0);
                    strcat(ficheiro, ".gz");
                    execlp("sha1sum", "sha1sum", ficheiro, NULL);
                }
            }
        }
        else {
            waitpid(-1,&status,0);
            printf("pai\n");
            close(fd[1]);
            read(fd[0],&shaisum,160);
            c = strtok(shaisum," ");
            strcpy(shaisum,c);
            printf("%s\n",shaisum);
            while(read(fd[0],&c,1)>0) {ficheiro[i]=*c;i++;}
            if(strcmp(comando,"backup")){backupficheiro(ficheiro,shaisum,msg.pid);}
            else exit(3);
        }

    }
    unlink(fifo);
    return 0;
}
