#include "file.h"
#include "mtk_c.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE_ENTRY file_table[NUM_FILE];

/***********************************
 * @brief ファイルテーブルのデバッグ
 **********************************/
void print_file_table() {
    for (FILE_ID_TYPE id = 0; id < NUM_FILE; id++) {
        printf(
            "No.%d name: %s, size: %d, buffer: %s, semaphore_id: %d\n",
            id,
            file_table[id].name,
            file_table[id].size,
            file_table[id].buffer,
            file_table[id].semaphore_id);
    }
    printf("\n");
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
 **********************************/
void touch(const char* filename) {
    check_null(filename);
    for (FILE_ID_TYPE id = 0; id < NUM_FILE; id++) {
        if (file_table[id].name[0] == '\0') {
            copy_string(filename, file_table[id].name, sizeof(file_table[id].name));
            file_table[id].size = 0;
            break;
        }
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
 * @brief 安全に文字列をコピーする
 * @param from コピー元の文字列
 * @param to コピー先の文字列バッファ
 * @param to_size コピー先のバッファサイズ(関数内だとポインタのサイズを得てしまうため関数外でサイズを指定する必要がある)
 **********************************/
void copy_string(const char* from, char* to, int to_size) {
    if (to_size == 0) {
        fprintf(stderr, "Copy Error: (copy_string) to_size is 0\n");
        exit(EXIT_FAILURE);
    }
    strncpy(to, from, to_size - 1);
    to[to_size - 1] = '\0';  // 終端文字を必ず設定
}

/***********************************
 * @brief ファイルの編集
 * @param filename: 文字配列(の先頭アドレス)
 **********************************/
void edit(const char* filename) {
    check_null(filename);
    FILE_ID_TYPE id = search_file_id(filename);
	if (id == -1) {
		printf("File Not Found\n");
		return;
	}
    if (strcmp(file_table[id].name, filename) == 0) {
        char buffer[257] = {0};
        write_lines(buffer);
        write_to_file(buffer, id);
    }
}

/***********************************
 * @brief 入力を1行ずつバッファに書き込む
 * @param filename: 文字配列(の先頭アドレス)
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
 * @brief 入力を1行ずつバッファに書き込む
 * @param buffer: 文字配列(の先頭アドレス)
 **********************************/
void write_lines(char* buffer) {
    check_null(buffer);
    char temp[257];
    printf("\n:Write Mode: You can edit a file.\n");
    printf("If you wanna change to Command Mode, Please push [Esc].\n\n");
    while (1) {
		if (fgets(temp, sizeof(temp), stdin) == NULL) { // sizeof(temp)以内でtempに格納
            fprintf(stderr, "IO Error: (write_lines) fgets failed\n");
            exit(EXIT_FAILURE);
		}
		char* esc_ptr = strchr(temp, '\v');
        if (esc_ptr) {  // Escが押されたら(csys68k.cでEsc入力でバッファに\vを入力)
            *esc_ptr = '\0';
            strcat(buffer, temp);
			fflush(stdin); // fgetsの入力が257字を超えた分がstdinに残っているため破棄
            break;
        }
        strcat(buffer, temp);
    }
}

/***********************************
 * @brief 入力をファイルテーブル内の指定したファイルに書き込む
 * @param buffer: 文字配列(の先頭アドレス)
 * @param id: ファイルテーブルのインデックス
 **********************************/
void write_to_file(const char* buffer, FILE_ID_TYPE id) {
    check_null(buffer);
    if (file_table[id].size == 0 && file_table[id].buffer[0] == '\0') {
        file_table[id].size = strlen(buffer);
		int semaphore_id = file_table[id].semaphore_id;
		P(semaphore_id);
        copy_string(buffer, file_table[id].buffer, sizeof(file_table[id].buffer));
		V(semaphore_id);
    }
}

/***********************************
 * @brief ファイルの削除
 * @param filename: 文字配列(の先頭アドレス)
 **********************************/
void rm(const char* filename) {
    check_null(filename);
    FILE_ID_TYPE id = search_file_id(filename);
    if (strcmp(file_table[id].name, filename) == 0) {
        memset(&file_table[id], 0, sizeof(FILE_ENTRY));
        file_table[id].size = UNDEFINED_SIZE;
    }
}

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
