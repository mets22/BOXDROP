#include "server.h"



int fileExists(char *file)
{
    int fd[2],x,temp,i,tline;
    char* slink,*templine,*line;
    char *pathofbackup = getPathOfBackupFolder();
    pipe(fd);


    if(x=fork()==0){
      close(fd[1]);
      dup2(fd[0],1);
      while(read(fd[0],slink,1)!=0){
        readlink(slink,templine,tline);
        line=strndup(templine,tline);
        if(strcmp(line,file)==0) return 1;
      }
      return 0;
    }else{
        close(fd[0]);
        dup2(fd[1],0);
        execlp("find","find","-L",pathofbackup,"-xtype","l","-printf","%f",0);
        fprintf(stderr, "Erro na procura de ficheiros\n");
        exit(EXIT_FAILURE);
    }
}

void backupfile(char *file, char *shaisum)
{
    int status,pidp[4],i=0;
    char aux[128],*c;
    char * newfile = getPathOfDataFolder();
    char * linklocation = getPathOfMetadataFolder();
    strcpy(aux, file);
    strcat(linklocation, aux);
    strcat(aux,".gz");
    c=strtok(shaisum,"\n");
    strcat(newfile, c);
    strcat(newfile,".gz");


    if(pidp[i++]=fork()==0) {
        execlp("mv", "mv", aux, newfile, NULL);
        fprintf(stderr, "Erro na transferência de ficheiro\n");
        exit(EXIT_FAILURE);
    }
    else {
        waitpid(pidp[0], &status, 0);

        if (pidp[i++] = fork() == 0) {
            execlp("ln", "ln", "-s", newfile, linklocation, NULL);
            fprintf(stderr, "Erro na criação de link\n");
            exit(EXIT_FAILURE);
        }
        else{
            waitpid(pidp[1],&status,0);
            exit(3);
        }
    }
}

void restorefile(char *file)
{
    char *dat, *meta, actual[200], shaisum[160],*c;
    int x,status,i=0, fd[2];
    pid_t pid1,pid2;
    int error;
    char *nameoffile = (char *) malloc(sizeof(char)*128);

    dat =(char *) malloc(sizeof(char)*128);
    //vai buscar a pasta da metadata
    meta=getPathOfMetadataFolder();
    strcpy(dat,getPathOfDataFolder());
    pipe(fd);
    if(fork()==0) {
        close(fd[0]);
        dup2(fd[1], 1);
        execlp("sha1sum","sha1sum",file,NULL);
    }
    else {
        close(fd[1]);
        read(fd[0], &shaisum, 160);
        c=strtok(shaisum," ");
        strcat(dat,c);
        //vai buscar directoria actual
        getcwd(actual, sizeof(actual));
        strcat(nameoffile, actual);
        strcat(nameoffile, "/");
        strcat(nameoffile, file);
        strcat(nameoffile, ".gz");
        strcat(meta, file);

        //vai buscar directoria para onde aponta o atalho
        printf("meta:%s\ndat: %s\n", meta, dat);
        x = readlink(meta, dat, 128);
        c=strtok(nameoffile,"\n");
        strcpy(nameoffile,c);
        if (pid1 = fork() == 0) {
            execlp("cp", "cp", dat, nameoffile, NULL);
        }
        else {
            waitpid(pid1, &status, 0);

            if (pid2 = fork() == 0) {
                execlp("gunzip", "gunzip", "-d", nameoffile, 0);
                fprintf(stderr, "Erro ao extrair ficheiro\n");
            }
            else {
                waitpid(pid2, &status, 0);
                exit(3);
            }
        }
    }
}

int main()
{
    int privatefifo, dummyfifo, publicfifo, n, fd[2], pid,status,i=0;
    struct message msg;
    static char buffer[PIPE_BUF];
    char comando[10], ficheiro[128], *c,*ch, shaisum[160];
    char * fifo;

    createBackupFolders();
    fifo = getPathOfBackupFolder();
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

    if( (publicfifo = open(fifo, O_RDONLY)) < 0 ||
        (dummyfifo = open(fifo, O_WRONLY | O_NDELAY)) < 0) {
        perror(fifo);
        exit(1);
    }
    /*Read the message from PUBLIC fifo*/
    while(read(publicfifo, &msg, sizeof(msg)) > 0) {
        printf("MENSAGEM!\n");
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
                        dup2(fd[1], 1);
                        close(fd[0]);
                        if (fork() == 0) execlp("sha1sum", "sha1sum", ficheiro, NULL);
                        else {
                            wait(&status);
                            execlp("gzip", "gzip", "-k", ficheiro, NULL);
                        }
                    }
                }
                else {
                    waitpid(-1, &status, 0);
                    close(fd[1]);
                    read(fd[0], &shaisum, 160);
                    c = strtok(shaisum, " ");
                    strcpy(shaisum, c);
                    c = strtok(NULL, "\n");
                    if (fork() == 0) backupfile(ficheiro, shaisum);
                    else waitpid(-1,&status,0);
                }
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
              if(fork()==0) restorefile(ficheiro);
              else waitpid(-1,&status,0);
          }
        }
        kill(msg.pid, SIGCONT);
    }
    unlink(fifo);
    return 0;
}
