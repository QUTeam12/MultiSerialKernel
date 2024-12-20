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

void print_file_table();
void init_file_table();
void touch(const char* filename);
void check_null(const char* ptr);
void copy_string(const char* from, char* to, int to_size);
void edit(const char* filename);
FILE_ID_TYPE search_file_id(const char* filename);
void write_lines(char* buffer);
void write_to_file(const char* buffer, FILE_ID_TYPE id);
void rm(const char* filename);
