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

#define BUFFER_SIZE 256+ 1 // 確保するバイトに終端文字\0を含むため17バイトで16文字まで
#define COMMAND_SIZE 8 + 1
#define FILENAME_SIZE 16 + 1 
#define NULL_SEMAPHORE_ID -1
#define NUM_FILE 8
#define UNDEFINED_SIZE -1

typedef unsigned int FILE_ID_TYPE;

typedef struct {
        char name[16 + 1];  // ファイル名
        int size;           // ファイルサイズ
        char buffer[256 + 1];   // ファイルデータ
        int semaphore_id;  // 排他制御用セマフォID
} FILE_ENTRY;

extern void outbyte(int port, unsigned char c);
extern char inbyte(int port);
extern void sleep();

void init_file_table();
void terminal_mode(FILE* r_w_stream);
void execute_command(const char* command, const char* filename, FILE* r_w_stream);
void print_commands(FILE* w_stream);
void print_file_table(FILE* w_stream);
void touch(const char* filename, FILE* w_stream);
void rm(const char* filename, FILE* w_stream);
FILE_ID_TYPE search_file_id(const char* filename);
void edit(const char* filename, FILE* r_w_stream);
void write_mode(FILE* r_w_stream, FILE_ID_TYPE id, const unsigned int is_backline);
void read_mode(FILE* r_w_stream, FILE_ID_TYPE id);
unsigned int get_port(FILE* stream);
void out_no_newline_end(int port, const char* buf, const unsigned int buf_size);
void input_command(char* command, char* file, FILE* r_stream);
void input(char* buf, const unsigned int buf_size, FILE* r_stream);
void split(const char* from, const unsigned int from_size, char* first, const unsigned int first_size, char* second, const unsigned int second_size, const char *sep);
void copy_string(const char* from, char* to, const unsigned int to_size);
void check_null(const char* ptr);
void sleep();
