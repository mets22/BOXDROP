//
// Created by mets on 29-04-2016.
//

#include "server.h"
#include "local.h"
#include "folderhandler.h"

const char *metadata = "/.Backup/metadata/";
const char *data = "/.Backup/data/";
char user[64];



int existeFicheiro(char *ficheiro)
{
    int fd[2],x,temp,i,tlinha;
    char* slink,templinha,linha;
    pipe(fd);

    /*execlp("find","find","home/user/.Backup/metadata",ficheiro,0);
    while(fgets(linha,20,0)!=NULL){
          if(strcmp(linha,ficheiro)==0) return 1;
    }*/
    // escrever para 0 ler de 1

    if(x=fork()==0){
      close(fd[1]);
      dup2(fd[0],1);
      while(read(fd[0],slink,1)!=0){
        readlink(slink,templinha,tlinha);
        linha=strndup(templinha,tlinha);
        //printf("%s\n",linha);
        if(strcmp(linha,ficheiro)==0) return 1;
      }
      printf("wrvwrv\n");
      return 0;
    }else{
        close(fd[0]);
        dup2(fd[1],0);
        execlp("find","find","-L","/home/goncalo/Documentos","-xtype","l","-printf","%f",0);

    }
}

void backupficheiro(char *ficheiro, char *shaisum)
{
    int status,pidp[4],i=0;
    char aux[128], newfile[512],linklocation[128],*c;

    printf("backupficheiro\n");
    strcpy(newfile, user);
    strcat(newfile, data);
    printf("backup - %s\n", newfile);
    strcpy(linklocation, user);
    strcat(linklocation, metadata);
    printf("backup - %s\n", linklocation);
    strcpy(aux, ficheiro);
    printf("backup - %s\n", aux);
    strcat(linklocation, aux);
    strcat(aux,".gz");
    printf("backup - %s\n", aux);
    c=strtok(shaisum,"\n");
    printf("backup - %s\n", c);
    strcat(newfile, c);
    printf("backup - %s\n", newfile);


    if(pidp[i++]=fork()==0) {
        execlp("mv", "mv", aux, newfile, NULL);
    }
    else {
        waitpid(pidp[0], &status, 0);

        if (pidp[i++] = fork() == 0) {
            execlp("ln", "ln", "-s", newfile, linklocation, NULL);
        }
        else{
            waitpid(pidp[1],&status,0);
        }
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
    createBackupFolders();
    strcpy(user,getenv("HOME"));
    strcpy(fifo,user);
    strcat(fifo,"/.Backup/");

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
        printf("MSG: %s\n", msg.cmd_line);
        pipe(fd);
        c=strtok(msg.cmd_line," ");
        strcpy(comando,msg.cmd_line);
        if (strcmp(comando, "backup") == 0) {
            while (c = strtok(NULL, " ")) {
                n++;
                strcpy(ficheiro, c);
                if (n > 5) {
                    waitpid(0, &status, 0);
                    n--;
                }
                if ((pid = fork()) == 0) {
                    if (privatefifo = open(msg.fifo_name, O_WRONLY | O_NDELAY) == -1) {
                        printf("%d\n", privatefifo);
                        perror(msg.fifo_name);
                        exit(1);
                    }
                    else {
                        printf("else\n");

                        dup2(fd[1], 1);
                        close(fd[0]);
                        if (fork() == 0) execlp("sha1sum", "sha1sum", ficheiro, NULL);
                        else {
                            waitpid(-1, &status, 0);
                            execlp("gzip", "gzip", ficheiro, NULL);
                        }
                    }
                }
                else {
                    waitpid(-1, &status, 0);
                    printf("pai\n");
                    close(fd[1]);
                    read(fd[0], &shaisum, 160);
                    c = strtok(shaisum, " ");
                    strcpy(shaisum, c);
                    printf("%s\n", shaisum);
                    printf("%s\n", ficheiro);
                    backupficheiro(ficheiro, shaisum);
                }
            }
            kill(msg.pid, SIGCONT);
        }
    }
    unlink(fifo);
    return 0;
}
