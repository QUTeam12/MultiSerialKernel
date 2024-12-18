#define TEST2
#include "task.h"
#include "mtk_c.h"
#include <stdio.h>


int main(){
	setfd();	
	init_kernel();
	set_task(task2);
	set_task(task1);
	set_task(task3);
	begin_sch();
	return 0;
}
