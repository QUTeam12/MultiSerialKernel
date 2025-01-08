#include "file.h"

#include <stddef.h>  // NULL, size_t
#include <stdio.h>   // FILE, fileno
#include <stdlib.h>  // EXIT_FAILURE, exit
#include <string.h>  // memset, strcpy, strncpy, strlen, strcmp

#include "mtk_c.h"

FILE_ENTRY file_table[NUM_FILE];

extern SEMAPHORE_TYPE semaphore[NUMSEMAPHORE];

/***********************************
 * @brief ファイルテーブルの初期化
 **********************************/
void init_file_table() {
    for (FILE_ID_TYPE id = 0; id < NUM_FILE; id++) {
        memset(&file_table[id], 0, sizeof(FILE_ENTRY));  // FILE_ENTRYのメモリを0で埋める
        file_table[id].size = UNDEFINED_SIZE; // 空ファイルのサイズ(0)とファイル未作成のサイズ(-1)を区別
        file_table[id].semaphore_id = id;
    }
}

/***********************************
 * @brief ターミナルの起動
 * @param r_w_stream: 読み書き対応ファイルストリーム
***********************************/
void terminal_mode(FILE* r_w_stream) {
    fprintf(r_w_stream, "\n\n:Terminal Mode: You have some options.\n");
    fprintf(r_w_stream, "If you wanna know commands, please type [help].\n");
    char command[COMMAND_SIZE];
    char filename[FILENAME_SIZE];
    input_command(command, filename, r_w_stream);
    execute_command(command, filename, r_w_stream);
}

/***********************************
 * @brief コマンド入力を受け取る
 * @param command: 文字配列(の先頭アドレス)
 * @param file: 文字配列(の先頭アドレス)
 * @param r_stream: 読み込み対応ストリーム
 **********************************/
void input_command(char* command, char* file, FILE* r_stream) {
    const unsigned int buf_size = COMMAND_SIZE + FILENAME_SIZE - 1; // 余剰な\0考慮分を引く
    char buf[buf_size];
    input(buf, buf_size, r_stream);
    split(buf, buf_size, command, COMMAND_SIZE, file, FILENAME_SIZE, " ");
}

/***********************************
 * @brief コマンドの実行
 * @param r_w_stream: 読み書き対応ファイルストリーム
***********************************/
void execute_command(const char* command, const char* filename, FILE* r_w_stream) {
    if (strcmp(command, "help") == 0) {
        print_commands(r_w_stream);
    } else if (strcmp(command, "ls") == 0) {
        print_file_table(r_w_stream);
    } else if (strcmp(command, "touch") == 0) {
        touch(filename, r_w_stream);
    } else if (strcmp(command, "rm") == 0) {
        rm(filename, r_w_stream);
    } else if (strcmp(command, "edit") == 0) {
        edit(filename, r_w_stream);
    } else {
        fprintf(r_w_stream, "\nInvalid Command. Please push again.\n");
    }
}

/***********************************
 * @brief コマンド一覧の出力
 * @param w_stream: 書き込み対応ファイルストリーム
 **********************************/
void print_commands(FILE* w_stream) {
    fprintf(w_stream, "\n:Commands:\n");
    fprintf(w_stream, "[help] You can see the commands you execute\n");
    fprintf(w_stream, "[ls] You can see the file table\n");
    fprintf(w_stream, "[touch filename] You can create the file(by 16 characters)\n");
    fprintf(w_stream, "[rm filename] You can delete the file\n");
    fprintf(w_stream, "[edit filename] You can edt the file\n");
    fprintf(w_stream, "#Attention# The user in Write mode is only one by a file.\n");
    fprintf(w_stream, "If someone else is editing the file, you switch to Read Mode.\n\n");
}

/***********************************
 * @brief ファイルテーブルの出力
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
 * @brief ファイルの作成
 * @param filename: 文字配列(の先頭アドレス)
 * @param w_stream: 書き込み対応ファイルストリーム
 **********************************/
void touch(const char* filename, FILE* w_stream) {
    check_null(filename);
    if (is_filename_empty(filename)) {
	fprintf(w_stream, "\nThe filename is empty. Please type again.\n");
        return;
    }
    if (is_file_exists(filename)) {
	fprintf(w_stream, "\nThe file already exists. Please type another name.\n");
        return;
    }
    for (FILE_ID_TYPE id = 0; id < NUM_FILE; id++) {
        if (is_filename_empty(file_table[id].name)) {
            copy_string(filename, file_table[id].name, sizeof(file_table[id].name));
            file_table[id].size = 0;
	    fprintf(w_stream, "\nThe file was created.\n");
            return;
        }
    }
    fprintf(w_stream, "\nFile Table is Full. Please delete some files.\n");
}

/***********************************
 * @brief ファイル名が空かどうか
 * @param filename: 文字配列(の先頭アドレス)
 * @return: true(1) or false(0)
 **********************************/
unsigned int is_filename_empty(const char* filename) {
    return filename[0] == '\0';
}

/***********************************
 * @brief ファイルが存在するかどうか
 * @param filename: 文字配列(の先頭アドレス)
 * @return: true(1) or false(0)
 **********************************/
unsigned int is_file_exists(const char* filename) {
    for (FILE_ID_TYPE id = 0; id < NUM_FILE; id++) {
        if (strcmp(file_table[id].name, filename) == 0) {
            return 1;
        }
    }
    return 0;
}

/***********************************
 * @brief ファイルの削除
 * @param filename: 文字配列(の先頭アドレス)
 * @param w_stream: 書き込み対応ファイルストリーム
 **********************************/
void rm(const char* filename, FILE* w_stream) {
    check_null(filename);
    if (is_filename_empty(filename)) {
	fprintf(w_stream, "\nThe filename is empty. Please type again.\n");
        return;
    }
    FILE_ID_TYPE id = search_file_id(filename);
    if (id == -1) {
        fprintf(w_stream, "\nFile Not Found\n");
        return;
    }
    memset(&file_table[id], 0, sizeof(FILE_ENTRY));
    file_table[id].size = UNDEFINED_SIZE;
    file_table[id].semaphore_id = id;
    fprintf(w_stream, "\nThe file was deleted.\n");
}

/***********************************
 * @brief ファイル名からファイルIDを探す
 * @param filename: 文字配列(の先頭アドレス)
 **********************************/
FILE_ID_TYPE search_file_id(const char* filename) {
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
    if (is_filename_empty(filename)) {
	fprintf(r_w_stream, "\nThe filename is empty. Please type again.\n");
	return;
    }
    FILE_ID_TYPE id = search_file_id(filename);
    if (id == -1) {
        fprintf(r_w_stream, "\nFile Not Found\n");
        return;
    }
    write_mode(r_w_stream, id, 0);
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
    out_no_newline_end(port, buf, file_size); // 終端に改行がない場合があるためfprintfは駄目

    for (; file_table[id].size < sizeof(file_table[id].buffer); file_table[id].size++) {
        char c = inbyte(port);
        switch (c) {
            case '\r':  // CRの場合
            case '\n':  // LFの場合
                outbyte(port, '\r');
                outbyte(port, '\n');
                *(buf + file_table[id].size) = '\n';
		break;
            case '\x1b':  // Escの場合
		command_mode(r_w_stream, id);
		return;
            case '\x7f':  // バックスペースの場合
                if (file_table[id].size > 0) {
		    file_table[id].size--;
		    if (*(buf + file_table[id].size) == '\n') { // 1行上に戻る場合
                        *(buf + file_table[id].size) = '\0'; // readにはないがreadの外で\nの後に恐らく実装されている
			write_mode(r_w_stream, id, 1); // outbyteだけでは一行上に干渉できないため再帰。設計的に良くない。
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
 * @brief 入力文字に応じてWrite Modeを終了or再開
 * @param r_w_stream: 読み書き対応ファイルストリーム
 * @param id: ファイルテーブルのインデックス
 **********************************/
void command_mode(FILE* r_w_stream, FILE_ID_TYPE id) {
    fprintf(r_w_stream, "\n\n:Command Mode: You have two options.\n");
    fprintf(r_w_stream, "[Enter] Finish editing\n");
    fprintf(r_w_stream, "[w] Restart Write Mode\n");
    const int semaphore_id = file_table[id].semaphore_id;
    while (1) {
        switch (inbyte(get_port(r_w_stream)) {
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
		break;
        }
    }
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
	    return;
	}
    	fprintf(r_w_stream, "\n\n:Read Mode: Someone else is in Write Mode. So you need to wait.\n");
    	fprintf(r_w_stream, "You can see the content of a file even if someone else is editing.\n");
    	fprintf(r_w_stream, "#Attention# The user in Write mode is only one by a file.\n");
    	fprintf(r_w_stream, "If someone else switch from Write Mode, you switch to it.\n\n");
	out_no_newline_end(get_port(r_w_stream), file_table[id].buffer, file_table[id].size); // 終端に改行がない場合があるためfprintfは駄目
	sleep(); // Cのタイマ関連のライブラリが使用不可能なのと自前実装の場合最適化によりタイマ実装不可能なのでアセンブリ実装
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
	case 1:
	case 2:
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
 * @brief 安全に入力を受け取る
 * @param buf: 文字配列(の先頭アドレス)
 * @param buf_size: 文字配列のサイズ
 * @param r_stream: 読み込み対応ストリーム
 **********************************/
void input(char* buf, const unsigned int buf_size, FILE* r_stream) {
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
 * @brief 安全に文字列を2分割する(区切り文字がない場合secondは空)
 * @param from 分割元の文字列
 * @param first: ２分割した前の文字列
 * @param second: ２分割した後の文字列
 * @param sep: 区切り文字列
**********************************/
void split(const char* from, const unsigned int from_size, char* first, const unsigned int first_size, char* second, const unsigned int second_size, const char *sep) {
	check_null(from);
	char temp_from[from_size];
	copy_string(from, temp_from, from_size);
	char* temp_first = strtok(temp_from, sep);
	if (temp_first == NULL) {
	　　fprintf(stderr, "Split Error: (split) strtok is failed\n");
	　　exit(EXIT_FAILURE);
	}
	char* temp_second = strtok(NULL, sep);
	if (temp_second == NULL) {
	　　second[0] = '\0';
        　　copy_string(temp_first, first, first_size);
        　　return;
	}
	copy_string(temp_first, first, first_size);
	copy_string(temp_second, second, second_size);	
}
	
	

/***********************************
 * @brief 安全に文字列をコピーする
 * @param from コピー元の文字列
 * @param to コピー先の文字列バッファ
 * @param to_size コピー先のバッファサイズ(関数内だとポインタのサイズを得てしまうため関数外でサイズを指定する必要がある)
**********************************/
void copy_string(const char* from, char* to, const unsigned int to_size) {
    check_null(from);
    if (to_size == 0) {
        fprintf(stderr, "Copy Error: (copy_string) to_size is 0\n");
        exit(EXIT_FAILURE);
    }
    strncpy(to, from, to_size - 1);
    to[to_size - 1] = '\0';  // 終端文字を必ず設定
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

