#include "file.h"

#include <stddef.h>  // NULL, size_t
#include <stdio.h>   // FILE, fileno
#include <stdlib.h>  // EXIT_FAILURE, exit
#include <string.h>  // memset, strcpy, strncpy, strlen, strcmp

#include "mtk_c.h"

FILE_ENTRY file_table[NUM_FILE];

/***********************************
 * @brief ファイルテーブルのデバッグ
 * @param w_stream: 書き込み対応ファイルストリーム
 **********************************/
void print_file_table(FILE* w_stream) {
    for (FILE_ID_TYPE id = 0; id < NUM_FILE; id++) {
        fprintf(
            w_stream,
            "No.%d name: %s, size: %d, buffer: %s, semaphore_id: %d\n",
            id,
            file_table[id].name,
            file_table[id].size,
            file_table[id].buffer,
            file_table[id].semaphore_id);
    }
    fprintf(w_stream, "\n");
}

/***********************************
 * @brief ファイルテーブルの初期化
 **********************************/
void init_file_table() {
    for (FILE_ID_TYPE id = 0; id < NUM_FILE; id++) {
        memset(&file_table[id], 0, sizeof(FILE_ENTRY));  // FILE_ENTRYのメモリを0で埋める
        file_table[id].size = UNDEFINED_SIZE;
        // file_table[id].buffer = NULL;
        file_table[id].semaphore_id = id;
    }
}

/***********************************
 * @brief ファイルの作成
 * @param filename: 文字配列(の先頭アドレス)
 * @param w_stream: 書き込み対応ファイルストリーム
 **********************************/
void touch(const char* filename, FILE* w_stream) {
    check_null(filename);
    check_null((char*)w_stream);
    for (FILE_ID_TYPE id = 0; id < NUM_FILE; id++) {
        if (file_table[id].name[0] == '\0') {
            copy_string(filename, file_table[id].name, sizeof(file_table[id].name));
            file_table[id].size = 0;
            return;
        }
    }
    fprintf(w_stream, "File Not Found\n");
}

/***********************************
 * @brief 安全に文字列をコピーする
 * @param from コピー元の文字列
 * @param to コピー先の文字列バッファ
 * @param to_size コピー先のバッファサイズ(関数内だとポインタのサイズを得てしまうため関数外でサイズを指定する必要がある)
**********************************/
void copy_string(const char* from, char* to, size_t to_size) {
    check_null(from);
    check_null(to);
    if (to_size == 0) {
        fprintf(stderr, "Copy Error: (copy_string) to_size is 0\n");
        exit(EXIT_FAILURE);
    }
    strncpy(to, from, to_size - 1);
    to[to_size - 1] = '\0';  // 終端文字を必ず設定
}

/***********************************
 * @brief ファイルの削除
 * @param filename: 文字配列(の先頭アドレス)
 * @param w_stream: 書き込み対応ファイルストリーム
 **********************************/
void rm(const char* filename, FILE* w_stream) {
    check_null(filename);
    FILE_ID_TYPE id = search_file_id(filename);
    if (id == -1) {
        fprintf(w_stream, "File Not Found\n");
        return;
    }
    if (strcmp(file_table[id].name, filename) == 0) {
        memset(&file_table[id], 0, sizeof(FILE_ENTRY));
        file_table[id].size = UNDEFINED_SIZE;
    }
}

/***********************************
 * @brief 入力を1行ずつバッファに書き込む
 * @param filename: 文字配列(の先頭アドレス)
 * @return FILE_ID_TYPE: ファイルテーブルのインデックス
 **********************************/
FILE_ID_TYPE search_file_id(const char* filename) {
    check_null(filename);
    for (FILE_ID_TYPE id = 0; id < NUM_FILE; id++) {
        if (strcmp(file_table[id].name, filename) == 0) {
            return id;
        }
    }
    return -1;
}

/***********************************
 * @brief ファイルの編集
 * @param filename: 文字配列(の先頭アドレス)
 * @param r_w_stream: 読み書き対応ファイルストリーム
 **********************************/
void edit(const char* filename, FILE* r_w_stream) {
    check_null(filename);
    FILE_ID_TYPE id = search_file_id(filename);
    if (id == -1) {
        fprintf(r_w_stream, "File Not Found\n");
        return;
    }
    if (strcmp(file_table[id].name, filename) == 0) {
        write_to_file(r_w_stream, id);
    }
}

/***********************************
 * @brief 入力をファイルテーブル内の指定したファイルに書き込む
 * @param r_w_stream: 読み書き対応ファイルストリーム
 * @param id: ファイルテーブルのインデックス
 **********************************/
void write_to_file(FILE* r_w_stream, FILE_ID_TYPE id) {
    printf("\n:Write Mode: You can edit a file.\n");
    printf("If you wanna change to Command Mode, Please push [Esc].\n\n");

    int port = get_port(r_w_stream);
    char* buf = file_table[id].buffer;
    int buf_size = sizeof(file_table[id].buffer);
    int semaphore_id = file_table[id].semaphore_id;

    P(semaphore_id);
    for (int i = 0; i < buf_size; i++) {
        char c = inbyte(port);
        switch (c) {
            case '\r':  // CRの場合
            case '\n':  // LFの場合
                outbyte(port, '\r');
                outbyte(port, '\n');
                *(buf + i) = '\n';
            case '\x1b':  // Escの場合
                out_msg(port, "\r\n\r\n:Command Mode: You have some options.\r\n");
                out_msg(port, "[Enter] Finish editing\r\n");
                out_msg(port, "[w] Restart Write Mode\r\n");
                out_msg(port, "[r] Read Mode\r\n");
                out_msg(port, "#Attention# The user in Write mode is only one.\r\n\r\n");
                // from
                char command = inbyte(port);
                switch (command) {
                    case '\r':
                    case '\n':
                        V(semaphore_id);
                        return;
                    case 'w':
                        out_msg(port, "\r\n\r\n:Write Mode: You can edit a file.\r\n");
                        i--;
                        continue;
                    case 'r':
                        V(semaphore_id);
                        out_msg(port, "\r\n\r\n:Read Mode: The user in Write mode is only one.\r\n");
                        out_msg(port, "If you wanna switch to Write Mode, Please push [Esc].\r\n\r\n");
                        break;
                }
                // to
            case '\x7f':  // バックスペースの場合
                if (i > 0) {
                    outbyte(port, '\x8');
                    outbyte(port, ' ');
                    outbyte(port, '\x8');
                    i--;
                }
                i--;
                break;
            default:  // 通常文字の場合
                outbyte(port, c);
                *(buf + i) = c;
                break;
        }
    }
    fprintf(stderr, "Buffer Overflow Error: (read_chars) buf is overflowed\n");
    exit(EXIT_FAILURE);
}

/***********************************
 * @brief ストリームのファイルディスクリプタからポートを返す
 * @param stream: ファイルストリーム
**********************************/
int get_port(FILE* stream) {
    int fd = fileno(stream);
    check_fd(fd);
    switch (fd) {
        case 0:
        case 3:
            return 0;
        case 4:
            return 1;
        default:
            fprintf(stderr, "Read Error: (read_lines) fd is invalid\n");
            exit(EXIT_FAILURE);
    }
}

/***********************************
 * @brief ポートに文字列を出力する
 * @param port: ポート番号
 * @param c: 出力する文字
 **********************************/
void out_msg(int port, const char* msg) {
    check_null(msg);
    while (*msg != '\0') {
        outbyte(port, *msg);
        msg++;
    }
}

/***********************************
 * @brief 文字配列の先頭アドレスがNULLの場合にエラーを出力して強制終了する
 * @param ptr: 文字配列(の先頭アドレス)
 **********************************/
void check_null(const char* ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Null Pointer Error: (check_null) ptr is nullptr\n");
        exit(EXIT_FAILURE);
    }
}

/***********************************
 * @brief 文字列のサイズが0の場合にエラーを出力して強制終了する
 * @param to_size: コピー先のバッファサイズ
 **********************************/
void check_size(size_t to_size) {
    if (to_size <= 0) {
        fprintf(stderr, "Read Error: (read_lines) to_size is 0 or negative\n");
        exit(EXIT_FAILURE);
    }
}

/***********************************
 * @brief カウントが負の値の場合にエラーを出力して強制終了する
 * @param fd: カウント
 **********************************/
void check_fd(int fd) {
    if (fd < 0) {
        fprintf(stderr, "File Descriptor Error: (check_fd) fd is negative\n");
        exit(EXIT_FAILURE);
    }
}

// TODO: bufをポインタにする場合
/*
void write_to_file(const char* buffer, FILE_ID_TYPE id) {
    check_null(buffer);
    size_t buffer_size = strlen(buffer) + 1;                 // 終端文字分を追加
    if (file_table[id].buffer == NULL) {                     // メモリが未割り当ての場合
        file_table[id].buffer = (char*)malloc(buffer_size);  // メモリを動的に割り当てる
        if (file_table[id].buffer == NULL) {
            fprintf(stderr, "memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    } else if (file_table[id].size < buffer_size) {                             // 既存のバッファが小さすぎる場合
        char* new_buffer = (char*)realloc(file_table[id].buffer, buffer_size);  // メモリを再割り当て
        if (new_buffer == NULL) {
            fprintf(stderr, "memory reallocation failed\n");
            exit(EXIT_FAILURE);
        }
        file_table[id].buffer = new_buffer;
    }

    file_table[id].size = buffer_size - 1;  // サイズを更新 (終端文字分を引く)
    strcpy(file_table[id].buffer, buffer);
}

void rm(const char* filename) {
    check_null(filename);
    FILE_ID_TYPE id = search_file_id(filename);
    if (strcmp(file_table[id].name, filename) == 0) {
        free(file_table[id].buffer);   // メモリを解放
        file_table[id].buffer = NULL;  // ポインタをNULLに初期化

        memset(&file_table[id], 0, sizeof(FILE_ENTRY));
    }
}
*/
