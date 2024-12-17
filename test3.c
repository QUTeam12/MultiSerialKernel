#define TEST2
#include "mtk_c.h"
#include <stdio.h>

FILE *com0in; /* UART1からの読み込み*/
FILE *com0out;/* UART1からの書き込み*/
FILE *com1in;/* UART2からの読み込み*/
FILE *com1out;/* UART2からの書き込み*/

void task1(){
	while(1){}
}

void setfd(){
	const char *read = "r";
	const char *write = "w";
	com0in = fdopen(3,read);
	com0out = fdopen(3, write);
	com1in = fdopen(4, read);
	com1out = fdopen(4, write);
}


int main(){
	setfd();	
	init_kernel();
	set_task(task1);
	begin_sch();
	return 0;
}
