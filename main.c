#include <stdio.h>

#include "file.h"
#include "mtk_c.h"

FILE* com0inout;  // UART1からの読み書き
FILE* com1inout;  // UART2からの読み書き

static int is_file_system_initialized = 0;

void set_fd() {
    const char* read_write = "w+";
    com0inout = fdopen(3, read_write);
    com1inout = fdopen(4, read_write);
}

void task1() {
    if (!is_file_system_initialized) {
        init_file_table();
        is_file_system_initialized = 1;
    }
    while (1) {
        select_command(com0inout);
    }
}

void task2() {
    while (1) {
        select_command(com1inout);
    }
}

int main() {
    init_kernel();
    set_fd();
    set_task(task1);
    set_task(task2);
    begin_sch();
}
