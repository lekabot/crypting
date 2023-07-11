#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE* log_init(char* filename) {
    FILE* log_file = fopen(filename, "w");
    return log_file;
}

int logcat(FILE* log_file, char* message, log_level level) {
    if (log_file == NULL) {
        return -1;
    }

    const char* level_str = NULL;
    switch (level) {
        case DEBUG:
            level_str = "DEBUG";
            break;
        case INFO:
            level_str = "INFO";
            break;
        case WARNING:
            level_str = "WARNING";
            break;
        case ERROR:
            level_str = "ERROR";
            break;
        case TRACE:
            level_str = "TRACE";
            break;
    }

    time_t current_time = time(NULL);
    char* timestamp = ctime(&current_time);
    timestamp[strlen(timestamp) - 1] = '\0';

    fprintf(log_file, "[%s] %s: %s\n", timestamp, level_str, message);
    fflush(log_file);
    return 0;
}

int log_close(FILE* log_file) {
    if (log_file == NULL) {
        return -1;
    }
    fclose(log_file);
    return 0;
}
