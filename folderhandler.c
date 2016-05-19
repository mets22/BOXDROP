#include "folderhandler.h"

char * getPathOfBackupFolder(){
    int res;
    char *string = (char *) malloc(sizeof(char)*strlen(HOME)+sizeof(char)*9);
    if((res = sprintf(string,"%s/.Backup/",HOME))<1){
        exit(-1);
    }
    return string;
}

char * getPathOfDataFolder(){
    int res;
    char *string = (char *) malloc(sizeof(char)*strlen(BACKUP)+sizeof(char)*5);
    if((res = sprintf(string,"%sdata/",BACKUP))<1){
        exit(-1);
    }
    return string;
}

char * getPathOfMetadataFolder(){
    int res;
    char *string = (char *) malloc(sizeof(char)*strlen(BACKUP)+sizeof(char)*9);
    if((res = sprintf(string,"%smetadata/",BACKUP))<1){
        exit(-1);
    }
    return string;
}

int createBackupFolders(){
    int error;
    int backup, data,metadata;
    if ((backup = mkdir(BACKUP,0777))!=0){
        error = errno;
        if(error == EEXIST){
            printf("Pasta /.Backup/ já existe\n");
        }
        else {
            printf("Erro ao criar a pasta de Backup\n");
            exit(EXIT_FAILURE);
        }
    }
    if((data=mkdir(DATA,0777))!=0){
        error = errno;
        if(error == EEXIST){
            printf("Pasta /data/ já existe\n");
        }
        else{
            printf("Erro ao criar a pasta de data\n");
            exit(EXIT_FAILURE);
        }
    }

    if((metadata=mkdir(METADATA,0777))!=0){
        error = errno;
        if(error == EEXIST){
            printf("Pasta /metadata/ já existe\n");
        }
        else{
            printf("Erro ao criar a pasta de matadata\n");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
