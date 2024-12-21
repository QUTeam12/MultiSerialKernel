#pragma once
/*
だめな場合
#ifndef IN_H
#define IN_H
...
#endif
*/

#include <stddef.h>  // size_t
#include <stdio.h>   // FILE

#define NULL_SEMAPHORE_ID -1
#define NUM_FILE 8
#define UNDEFINED_SIZE -1

typedef int FILE_ID_TYPE;

typedef struct {
        char name[16 + 1];  // ファイル名(確保するバイトに終端文字\0を含むため17バイトで16文字まで)
        int size;           // ファイルサイズ
        char buffer[257];   // ファイルデータ
        // char* buffer;       // ファイルデータのポインタ
        int semaphore_id;  // 排他制御用セマフォID
} FILE_ENTRY;

extern void outbyte(int port, unsigned char c);
extern char inbyte(int port);

void print_file_table(FILE* w_stream);
void init_file_table();
void touch(const char* filename, FILE* w_stream);
void copy_string(const char* from, char* to, size_t to_size);
void rm(const char* filename, FILE* w_stream);
FILE_ID_TYPE search_file_id(const char* filename);
void edit(const char* filename, FILE* r_w_stream);
void write_to_file(FILE* r_w_stream, FILE_ID_TYPE id);
int get_port(FILE* stream);
void out_msg(int port, const char* msg);
void check_null(const char* ptr);
void check_size(size_t to_size);
void check_fd(int fd);
