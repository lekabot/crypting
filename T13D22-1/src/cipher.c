#include <dirent.h>
#include <limits.h>
#include <openssl/des.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "logger.h"

#define MAX_PATH_LENGTH PATH_MAX
#define MAX_TEXT_LENGTH 1000

void print_file_contents(const char* filepath, FILE* log_file) {
    logcat(log_file, "File opened", INFO);

    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        logcat(log_file, "Failed to open file", ERROR);
        printf("n/a\n");
        return;
    }

    char line[MAX_TEXT_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

void append_to_file(const char* filepath, const char* text, FILE* log_file) {
    logcat(log_file, "File opened", INFO);

    FILE* file = fopen(filepath, "a");
    if (file == NULL) {
        logcat(log_file, "Failed to open file", ERROR);
        printf("n/a\n");
        return;
    }

    fprintf(file, "%s\n", text);
    fclose(file);
    logcat(log_file, "Text appended to file", INFO);
}

void caesar_encrypt(char* text, int shift) {
    int i = 0;
    while (text[i] != '\0') {
        char c = text[i];
        if (c >= 'a' && c <= 'z') {
            c = 'a' + (c - 'a' + shift) % 26;
        } else if (c >= 'A' && c <= 'Z') {
            c = 'A' + (c - 'A' + shift) % 26;
        }
        text[i] = c;
        i++;
    }
}

void clear_header_files(const char* directory, FILE* log_file) {
    logcat(log_file, "Directory opened", INFO);

    DIR* dir = opendir(directory);
    if (dir == NULL) {
        logcat(log_file, "Failed to open directory", ERROR);
        printf("n/a\n");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".h")) {
            size_t directory_length = strlen(directory);
            size_t name_length = strlen(entry->d_name);
            if (directory_length + 1 + name_length < MAX_PATH_LENGTH) {
                char filepath[MAX_PATH_LENGTH];
                strcpy(filepath, directory);
                filepath[directory_length] = '/';
                strcpy(filepath + directory_length + 1, entry->d_name);
                FILE* file = fopen(filepath, "w");
                if (file != NULL) {
                    fclose(file);
                }
            }
        }
    }
    closedir(dir);
    logcat(log_file, "Header files cleared", INFO);
}

void des_encrypt_file(const char* filepath, FILE* log_file) {
    logcat(log_file, "File opened", INFO);

    FILE* file = fopen(filepath, "r+");
    if (file == NULL) {
        logcat(log_file, "Failed to open file", ERROR);
        printf("n/a\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    unsigned char* buffer = (unsigned char*)malloc(size);
    if (buffer == NULL) {
        fclose(file);
        logcat(log_file, "Memory allocation failed", ERROR);
        printf("n/a\n");
        return;
    }

    size_t result = fread(buffer, 1, size, file);
    if (result != (size_t)size) {
        fclose(file);
        free(buffer);
        logcat(log_file, "Failed to read file", ERROR);
        printf("n/a\n");
        return;
    }

    DES_cblock key;
    DES_cblock iv;
    DES_key_schedule key_schedule;
    DES_random_key(&key);
    DES_random_key(&iv);
    DES_set_key_unchecked(&key, &key_schedule);
    DES_ncbc_encrypt(buffer, buffer, size, &key_schedule, &iv, DES_ENCRYPT);
    rewind(file);
    result = fwrite(buffer, 1, size, file);
    if (result != (size_t)size) {
        logcat(log_file, "Failed to write to file", ERROR);
        printf("n/a\n");
    }

    fclose(file);
    free(buffer);
    logcat(log_file, "File encrypted using DES", INFO);
}

int main() {
    char filepath[MAX_PATH_LENGTH];
    char text[MAX_TEXT_LENGTH];
    int exit_flag = 0;
    int file_specified = 0;
    FILE* log_file = log_init("log.txt");

    while (!exit_flag) {
        printf("Menu:\n");
        printf(" 1. Specify path to text file\n");
        printf(" 2. Enter text and append to file\n");
        printf(" 3. Encrypt .c files and clear .h files in a directory\n");
        printf(" 4. DES encrypt .c files and clear .h files in a directory\n");
        printf("-1. Exit\n");
        printf("Enter your choice: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("n/a\n");
            while (getchar() != '\n') continue;
        }
        switch (choice) {
            case 1:
                printf("Enter file path: ");
                getchar();
                fgets(filepath, sizeof(filepath), stdin);
                filepath[strcspn(filepath, "\n")] = '\0';
                if (strcmp(filepath, "-1") == 0) {
                    exit_flag = 1;
                    break;
                }
                file_specified = 1;
                print_file_contents(filepath, log_file);
                break;
            case 2:
                if (!file_specified) {
                    printf("n/a");
                    break;
                }
                printf("Enter text: ");
                getchar();
                fgets(text, sizeof(text), stdin);
                text[strcspn(text, "\n")] = '\0';
                if (strcmp(text, "-1") == 0) {
                    exit_flag = 1;
                    break;
                }
                append_to_file(filepath, text, log_file);
                print_file_contents(filepath, log_file);
                break;
            case 3:
                printf("Enter directory path: ");
                getchar();
                fgets(filepath, sizeof(filepath), stdin);
                filepath[strcspn(filepath, "\n")] = '\0';
                if (strcmp(filepath, "-1") == 0) {
                    exit_flag = 1;
                    break;
                }
                int shift;
                printf("Enter Caesar cipher shift: ");
                scanf("%d", &shift);
                if (shift == -1) {
                    exit_flag = 1;
                    break;
                }
                DIR* dir = opendir(filepath);
                if (dir == NULL) {
                    printf("n/a\n");
                    break;
                }
                struct dirent* entry;
                while ((entry = readdir(dir)) != NULL) {
                    if (entry->d_type == DT_REG && strstr(entry->d_name, ".c")) {
                        size_t directory_length = strlen(filepath);
                        size_t name_length = strlen(entry->d_name);
                        if (directory_length + 1 + name_length < MAX_PATH_LENGTH) {
                            char file_path[MAX_PATH_LENGTH];
                            strcpy(file_path, filepath);
                            file_path[directory_length] = '/';
                            strcpy(file_path + directory_length + 1, entry->d_name);
                            FILE* file = fopen(file_path, "r+");
                            if (file != NULL) {
                                fseek(file, 0, SEEK_END);
                                long size = ftell(file);
                                rewind(file);
                                char* buffer = malloc(size);
                                fread(buffer, 1, size, file);
                                caesar_encrypt(buffer, shift);
                                rewind(file);
                                fwrite(buffer, 1, size, file);
                                fclose(file);
                                free(buffer);
                            }
                        }
                    }
                }
                closedir(dir);
                clear_header_files(filepath, log_file);
                printf("Encryption and clearing complete\n");
                break;
            case 4:
                printf("Enter directory path: ");
                getchar();
                fgets(filepath, sizeof(filepath), stdin);
                filepath[strcspn(filepath, "\n")] = '\0';
                if (strcmp(filepath, "-1") == 0) {
                    exit_flag = 1;
                    break;
                }
                DIR* dir_des = opendir(filepath);
                if (dir_des == NULL) {
                    printf("n/a\n");
                    break;
                }
                struct dirent* entry_des;
                while ((entry_des = readdir(dir_des)) != NULL) {
                    if (entry_des->d_type == DT_REG && strstr(entry_des->d_name, ".c")) {
                        size_t directory_length_des = strlen(filepath);
                        size_t name_length_des = strlen(entry_des->d_name);
                        if (directory_length_des + 1 + name_length_des < MAX_PATH_LENGTH) {
                            char file_path_des[MAX_PATH_LENGTH];
                            strcpy(file_path_des, filepath);
                            file_path_des[directory_length_des] = '/';
                            strcpy(file_path_des + directory_length_des + 1, entry_des->d_name);
                            des_encrypt_file(file_path_des, log_file);
                        }
                    }
                }
                closedir(dir_des);
                clear_header_files(filepath, log_file);
                printf("DES encryption and clearing complete\n");
                break;
            case -1:
                exit_flag = 1;
                break;
            default:
                printf("n/a\n");
        }
        printf("\n");
    }

    log_close(log_file);

    return 0;
}
