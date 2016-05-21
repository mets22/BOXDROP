#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

char * getPathOfBackupFolder();
char * getPathOfDataFolder();
char * getPathOfMetadataFolder();
int createBackupFolders();

#define HOME getenv("HOME")
#define BACKUP getPathOfBackupFolder()
#define DATA getPathOfDataFolder()
#define METADATA getPathOfMetadataFolder()
