#include "file.h"

#include <stddef.h>  // NULL, size_t
#include <stdio.h>   // FILE, fileno
#include <stdlib.h>  // EXIT_FAILURE, exit
#include <string.h>  // memset, strcpy, strncpy, strlen, strcmp

#include "mtk_c.h"

FILE_ENTRY file_table[NUM_FILE];

extern SEMAPHORE_TYPE semaphore[NUMSEMAPHORE];

/***********************************
 * @brief ファイルテーブルのデバッグ
 * @param w_stream: 書き込み対応ファイルストリーム
 **********************************/
void print_file_table(FILE* w_stream) {
	fprintf(w_stream, "\n:File Table:\n");
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
        file_table[id].semaphore_id = id;
    }
}

/***********************************
***********************************/
void select_command(FILE* r_w_stream) {
    fprintf(r_w_stream, "\n\n:Select Mode: You have some options.\n");
    fprintf(r_w_stream, "[t] Touch: You can create file.\n");
    fprintf(r_w_stream, "[r] Remove: You can delete file.\n");
    fprintf(r_w_stream, "[e] Edit: You can edit file.\n");
    char filename[16];
    switch (inbyte(get_port(r_w_stream))) {
        case 't':
            fprintf(r_w_stream, "Please input a filename(16 characters).\n");
            input(filename, sizeof(filename), r_w_stream);
            touch(filename, r_w_stream);
			print_file_table(r_w_stream); // TODO: debug
            break;
        case 'r':
            fprintf(r_w_stream, "Please input the name of the file you wanna delete.\n");
            input(filename, sizeof(filename), r_w_stream);
            rm(filename, r_w_stream);
			print_file_table(r_w_stream); // TODO: debug
            break;
        case 'e':
            fprintf(r_w_stream, "Please input the name of the file you wanna edit.\n");
            input(filename, sizeof(filename), r_w_stream);
            edit(filename, r_w_stream);
			print_file_table(r_w_stream); // TODO: debug
            break;
        default:
            fprintf(r_w_stream, "\nInvalid Command. Please push again.\n");
    }
}

/***********************************
 * @brief ファイルの作成
 * @param filename: 文字配列(の先頭アドレス)
 * @param w_stream: 書き込み対応ファイルストリーム
 **********************************/
void touch(const char* filename, FILE* w_stream) {
    check_null(filename);
	if (filename[0] == '\0') {
		fprintf(w_stream, "\nThe filename is empty. Please type again.\n");
		return;
	}
    for (FILE_ID_TYPE id = 0; id < NUM_FILE; id++) {
        if (strcmp(file_table[id].name, filename) == 0) {
            fprintf(w_stream, "\nThe file already exists. Please type another name.\n");
            return;
        }
        if (file_table[id].name[0] == '\0') {
            copy_string(filename, file_table[id].name, sizeof(file_table[id].name));
            file_table[id].size = 0;
			fprintf(w_stream, "\nThe file was created.\n");
            return;
        }
    }
    fprintf(w_stream, "\nFile Table is Full. Please delete some files.\n");
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
	if (filename[0] == '\0') {
		fprintf(w_stream, "\nThe filename is empty. Please type again.\n");
		return;
	}
    FILE_ID_TYPE id = search_file_id(filename);
	if (id == -1) {
        fprintf(w_stream, "\nFile Not Found\n");
        return;
    }
    if (strcmp(file_table[id].name, filename) == 0) {
        memset(&file_table[id], 0, sizeof(FILE_ENTRY));
        file_table[id].size = UNDEFINED_SIZE;
		file_table[id].semaphore_id = id;
		fprintf(w_stream, "\nThe file was deleted.\n");
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
	if (filename[0] == '\0') {
		fprintf(r_w_stream, "\nThe filename is empty. Please type again.\n");
		return;
	}
    FILE_ID_TYPE id = search_file_id(filename);
    if (id == -1) {
        fprintf(r_w_stream, "\nFile Not Found\n");
        return;
    }
    if (strcmp(file_table[id].name, filename) == 0) {
        write_mode(r_w_stream, id, 0);
    }
}

/***********************************
 * @brief 入力をファイルテーブル内の指定したファイルに書き込む
 * @param r_w_stream: 読み書き対応ファイルストリーム
 * @param id: ファイルテーブルのインデックス
 * @param is_backline: 改行削除後のwriteかどうか。本当は関数設計的に良くないので消したい。
 **********************************/
void write_mode(FILE* r_w_stream, FILE_ID_TYPE id, const unsigned int is_backline) {
    const int semaphore_id = file_table[id].semaphore_id;
	if (is_backline == 0) {
		if (semaphore[semaphore_id].count == 0) {
			read_mode(r_w_stream, id);
			return;
		}
    	P(semaphore_id);
	}
    fprintf(r_w_stream, "\n\n:Write Mode: You can edit a file.\n");
    fprintf(r_w_stream, "If you wanna change to Command Mode, Please push [Esc].\n\n");
    const unsigned int port = get_port(r_w_stream);
    char* buf = file_table[id].buffer;
	unsigned int file_size = file_table[id].size;	
	out_no_newline_end(port, buf, file_size);

    for (file_table[id].size; file_table[id].size < sizeof(file_table[id].buffer); file_table[id].size++) {
        char c = inbyte(port);
        switch (c) {
            case '\r':  // CRの場合
            case '\n':  // LFの場合
                outbyte(port, '\r');
                outbyte(port, '\n');
                *(buf + file_table[id].size) = '\n';
				break;
            case '\x1b':  // Escの場合
                fprintf(r_w_stream, "\n\n:Command Mode: You have two options.\n");
                fprintf(r_w_stream, "[Enter] Finish editing\n");
                fprintf(r_w_stream, "[w] Restart Write Mode\n");
                while (1) {
                    switch (inbyte(port)) {
                        case '\r':
                        case '\n':
                            V(semaphore_id);
                            return;
                        case 'w':
                            V(semaphore_id);
							write_mode(r_w_stream, id, 0);
							return;
                        default:
                            fprintf(r_w_stream, "Invalid Command. Please push again.\n");
                    }
                }
				break;
            case '\x7f':  // バックスペースの場合
                if (file_table[id].size > 0) {
					file_table[id].size--;
					if (*(buf + file_table[id].size) == '\n') {
                		*(buf + file_table[id].size) = '\0'; // readにはないがreadの外で\nの後に多分実装されている
						write_mode(r_w_stream, id, 1); // outbyteだけでは一行上に干渉できない
						return;
					}
                	*(buf + file_table[id].size) = '\0';
                    outbyte(port, '\x8');
                    outbyte(port, ' ');
                    outbyte(port, '\x8');
                }
                file_table[id].size--;
                break;
			case '\x9': // タブの場合
				for (unsigned int j = 0; j < 4; j++) {
                	*(buf + file_table[id].size) = ' ';
					file_table[id].size++;
                    outbyte(port, ' ');
				}
				break;
            default:  // 通常文字の場合
                outbyte(port, c);
                *(buf + file_table[id].size) = c;
                break;
        }
    }
    fprintf(stderr, "Buffer Overflow Error: (write_mode) buf is overflowed\n");
    exit(EXIT_FAILURE);
}

/***********************************
 * @brief ファイルテーブル内の指定したファイルを読み込む
 * @param r_w_stream: 読み書き対応ファイルストリーム
 * @param id: ファイルテーブルのインデックス
**********************************/
void read_mode(FILE* r_w_stream, FILE_ID_TYPE id) {
    const int semaphore_id = file_table[id].semaphore_id;
    while (1) {
		if (semaphore[semaphore_id].count == 1) {
			write_mode(r_w_stream, id, 0);
			break;
		}
    	fprintf(r_w_stream, "\n\n:Read Mode: Someone else is in Write Mode. So you need to wait.\n");
    	fprintf(r_w_stream, "You can see the content of a file even if someone else is editing.\n");
    	fprintf(r_w_stream, "#Attention# The user in Write mode is only one by a file.\n");
    	fprintf(r_w_stream, "If someone else switch from Write Mode, you switch to it.\n\n");
		const char* buf = file_table[id].buffer;
		// fprintf(r_w_stream, "aiueo\nkakikukeko\n");
		// fprintf(r_w_stream, "%s\n", buf);
		out_no_newline_end(get_port(r_w_stream), file_table[id].buffer, file_table[id].size);
		sleep(r_w_stream, 3);
    }
}

/***********************************
 * @brief ストリームのファイルディスクリプタからポートを返す
 * @param stream: ファイルストリーム
**********************************/
unsigned int get_port(FILE* stream) {
    unsigned int fd = fileno(stream);
    switch (fd) {
        case 0:
        case 3:
            return 0;
        case 4:
            return 1;
        default:
            fprintf(stderr, "File Descriptor Error: (get_port) fd is invalid\n");
            exit(EXIT_FAILURE);
    }
}

/***********************************
 * @brief ポートに文字列を出力する(終端に改行をつける必要がない)
 * @param port: ポート番号
 * @param c: 出力する文字
 **********************************/
void out_no_newline_end(const int port, const char* buf, const unsigned int buf_size) {
    check_null(buf);
    for (unsigned int j = 0; j < buf_size; j++) {
        char outbyte_c = *(buf + j);
        if (outbyte_c == '\n') {
            outbyte(port, '\r');
            outbyte(port, '\n');
			continue;
        }
        outbyte(port, outbyte_c);
    }
}

/***********************************
 * @brief 入力を受け取る
 * @param buf: 文字配列(の先頭アドレス)
 * @param buf_size: 文字配列のサイズ
 * @param r_stream: 読み込み対応ストリーム
 **********************************/
void input(const char* buf, size_t buf_size, FILE* r_stream) {
    check_null(buf);
    if (fgets(buf, buf_size, r_stream) == NULL) {
        fprintf(stderr, "Input Error: (input) fgets is failed\n");
        exit(EXIT_FAILURE);
    }
	char* newline = strchr(buf,'\n');
	if (newline != NULL) {
		*newline = '\0';
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

void sleep(unsigned int seconds) {
    clock_t start_time = clock();
    clock_t end_time = start_time + (seconds * CLOCKS_PER_SEC);
    while (clock() < end_time) {
    }
}
