#include "server.h"


const char *metadata = "/.Backup/metadata/";
const char *data = "/.Backup/data/";
char user[64];



int existeFicheiro(char *ficheiro)
{
    int fd[2],x,temp,i,tlinha;
    char* slink,*templinha,*linha;
    pipe(fd);


    if(x=fork()==0){
      close(fd[1]);
      dup2(fd[0],1);
      while(read(fd[0],slink,1)!=0){
        readlink(slink,templinha,tlinha);
        linha=strndup(templinha,tlinha);
        if(strcmp(linha,ficheiro)==0) return 1;
      }
      return 0;
    }else{
        close(fd[0]);
        dup2(fd[1],0);
        execlp("find","find","-L","/home/nfernandes/.Backup/","-xtype","l","-printf","%f",0);
        fprintf(stderr, "Erro na procura de ficheiros\n");
        exit(EXIT_FAILURE);
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
            exit(3);
        }
    }
}

void restoreficheiro(char *ficheiro)
{
    char *dat, *meta, actual[200];
    int x,status,i=0;
    pid_t pid1,pid2;
    int error;
    char *nameoffile;

    dat =(char *) malloc(sizeof(char)*128);
    //vai buscar a pasta da metadata
    meta=getPathOfMetadataFolder();
    //vai buscar directoria actual
    getcwd(actual,sizeof(actual));

    strcat(meta,ficheiro);

    //vai buscar directoria para onde aponta o atalho
    x=readlink(meta,dat,128);

    if(pid1=fork()==0) {
        printf("copiar\n");
        execlp("cp","cp",dat,actual, NULL);
        fprintf(stderr, "Erro ao copiar ficheiros\n");
    }else {
        waitpid(pid1,&status,0);
        if (pid2=fork()==0) {
            printf("unzipar\n");
            execlp("gunzip","gunzip",ficheiro,0);
            fprintf(stderr, "Erro ao extrair ficheiro\n");
        }
        else{
            waitpid(pid2,&status,0);
        }
    }
}

int main()
{

    int privatefifo, dummyfifo, publicfifo, n, fd[2], pid[5],status,i=0;
    struct message msg;
    FILE *fin;
    static char buffer[PIPE_BUF];
    char comando[10], ficheiro[128], *c,*ch, shaisum[161], fifo[128];

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
        ch=strtok(msg.cmd_line," ");
        strcpy(comando,ch);
        if (n > 5) {
            waitpid(0, &status, 0);
            n--;
        }
        if (strcmp(comando, "backup") == 0) {
            while(ch = strtok(NULL, " ")) {
                n++;
                strcpy(ficheiro, ch);
                if (n > 5) {
                    waitpid(0, &status, 0);
                    n--;
                }
                if (fork() == 0) {
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
                            wait(&status);
                            execlp("gzip", "gzip", ficheiro, NULL);
                        }
                    }
                }
                else {
                    waitpid(0, &status, 0);
                    printf("pai\n");
                    close(fd[1]);
                    while(read(fd[0], &shaisum, 160)>0) {
                        c = strtok(shaisum, " ");
                        strcpy(shaisum, c);
                        //c = strtok(NULL, "\n");
                        printf("%s\n", shaisum);
                        printf("%s\n", ficheiro);
                        i++;
                        printf("I: %d\n", i);
                        if (fork() == 0) backupficheiro(ficheiro, shaisum);
                    }
                }

                printf("C: %s\n", c);
            }
        }
        else if(strcmp(comando, "restore") == 0)
        {
          while (c = strtok(NULL, " ")) {
              n++;
              strcpy(ficheiro, c);
              if (n > 5) {
                  waitpid(0, &status, 0);
                  n--;
              }
            }
            restoreficheiro(ficheiro);
        }
        kill(msg.pid, SIGCONT);
    }
    unlink(fifo);
    return 0;
}
