#include <stdio.h>

#include "file.h"
#include "mtk_c.h"

FILE* com0in;     // UART1からの読み込み
FILE* com0out;    // UART1からの書き込み
FILE* com0inout;  // UART1からの読み書き
FILE* com1in;     // UART2からの読み込み
FILE* com1out;    // UART2からの書き込み
FILE* com1inout;  // UART2からの読み書き

static int is_file_system_initialized = 0;

void set_fd() {
    const char* read = "r";
    const char* write = "w";
    const char* read_write = "w+";
    com0in = fdopen(3, read);
    com0out = fdopen(3, write);
    com0inout = fdopen(3, read_write);
    com1in = fdopen(4, read);
    com1out = fdopen(4, write);
    com1inout = fdopen(4, read_write);
}

void task1() {
    if (!is_file_system_initialized) {
        init_file_table();
        is_file_system_initialized = 1;
    }
    while (1) {
        char filename[16];
        fgets(filename, sizeof(filename), stdin);
        touch(filename, com0out);
        printf("filename is updated\n");
        print_file_table(com0out);
        edit(filename, com0inout);
        printf("buffer is updated\n");
        print_file_table(com0out);
    }
}

void task2() {
    fprintf(com1out, "\n:Read Mode: Someone else is Write Mode.\n");
    fprintf(com1out, "If you wanna switch to Write Mode, Please push [Esc].\n");
}

int main() {
    set_fd();
    init_kernel();
    set_task(task1);
    begin_sch();
}
