#ifndef LOGER_H
#define LOGER_H

#include <string.h>
#include <stdio.h>

typedef enum { DEBUG, INFO, WARNING, ERROR, TRACE }  log_level;

FILE* log_init(char *filename);
int logcat(FILE* log_file, char *message, log_level level);
int log_close(FILE* log_file);

#endif