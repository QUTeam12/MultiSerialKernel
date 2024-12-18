#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "file.h"

FILE_ENTRY file_table[NUM_FILE];

/***********************************
 * @brief ファイルテーブルのデバッグ
 **********************************/
void print_file_table() {	
	for (int i = 0; i < NUM_FILE; i++) {
		printf(
			"No.%d name: %s, size: %d, buffer: %s, semaphore_id: %d\n",
			i,
			file_table[i].name,
			file_table[i].size,
			file_table[i].buffer,
			file_table[i].semaphore_id
		);
	}
} 

/***********************************
 * @brief ファイルテーブルの初期化
 **********************************/
void init_file_table() {
	for (int i = 0; i < NUM_FILE; i++) {
		file_table[i].name[0] = '\0'; // 文字配列の先端に終端文字を入れて空を表現
		file_table[i].size = UNDEFINED_SIZE;
		file_table[i].buffer[0] = '\0';
		file_table[i].semaphore_id = i;
	}
}

/***********************************
 * @brief ファイルの作成
 * @param name: 文字配列(の先頭アドレス)
 **********************************/
void touch(char *filename) {
	for (int i = 0; i < NUM_FILE; i++) {
		if (file_table[i].name[0] == '\0') {
			strcpy(file_table[i].name, filename);
			file_table[i].size = 0;
			break;
		}
	}
}


/***********************************
 * @brief ファイルの編集
 * @param name: 文字配列(の先頭アドレス)
 **********************************/
void edit(char *filename) {
	for (int i = 0; i < NUM_FILE; i++) {
		if (strcmp(file_table[i].name, filename) == 0) {
			char buffer[256];
			scanf("%s", buffer);
			if (file_table[i].buffer[0] == '\0') {
				file_table[i].size = strlen(buffer);
				if (buffer != NULL) {
					strcpy(file_table[i].buffer, buffer);
				}
				break;
			}
		}
	}
}

