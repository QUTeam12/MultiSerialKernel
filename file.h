#define NULL_SEMAPHORE_ID -1
#define NUM_FILE 8
#define UNDEFINED_SIZE -1

typedef struct {
	char name[16];          // ファイル名
	int size;               // ファイルサイズ
	char* buffer[256];            // ファイルデータへのポインタ
	int semaphore_id;      // 排他制御用セマフォID
} FILE_ENTRY;

void print_file_table();
void init_file_table();
void touch(char *filename);
void edit(char *filename);

