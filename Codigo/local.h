//
// Created by mets on 16-05-2016.
//

#ifndef BOXDROP_LOCAL_H
#define BOXDROP_LOCAL_H

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define B_SIZE (PIPE_BUF)

const char *PUBLIC = "publicfifo";

struct message {
    char cmd_line[B_SIZE];
    int pid;
};

#endif //BOXDROP_LOCAL_H
