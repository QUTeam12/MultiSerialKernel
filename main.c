#include <stdio.h>

#include "file.h"
#include "mtk_c.h"

FILE* com0in;  // UART1からの読み込み
FILE* com0out; // UART1からの書き込み
FILE* com1in;  // UART2からの読み込み
FILE* com1out; // UART2からの書き込み

void set_fd() {
    const char* read = "r";
    const char* write = "w";
    com0in = fdopen(3, read);
    com0out = fdopen(3, write);
    com1in = fdopen(4, read);
    com1out = fdopen(4, write);
}

void task1() {
    init_file_table();
    while (1) {
        char filename[] = "newfile";
        touch(filename);
        printf("filename is updated\n");
        print_file_table();
        edit(filename);
        printf("buffer is updated\n");
        print_file_table();
		// rm(filename);
        // printf("file is deleted\n");
        // print_file_table();
    }
}

void task2() {
	fprintf(com1out, "\n:Read Only Mode: Someone else is Write Mode.\n");
	fprintf(com1out, "If you wanna switch to Write Mode, Please push [Esc].\n");
}

int main() {
    set_fd();
    init_kernel();
    set_task(task1);
    begin_sch();
}
