#include "task.h"
#include "mtk_c.h"
#include <stdio.h>

FILE *com0in;
FILE *com0out;
FILE *com1in;
FILE *com1out;

void setfd(){
	const char *read = "r";
	const char *write = "w";
	com0in = fdopen(3, read);
	com0out = fdopen(3, write);
	com1in = fdopen(4, read);
	com1out = fdopen(4, write);
}
volatile int i = 0;
volatile int end = 0;
char s0[256];
char s1[256];

void task1(){
	while(1){
		P(0);
		while(1){
			printf("a1\n");
			fscanf(com0in, "%s",s0);
			printf("a2\n");
			P(1);
			i++;
			V(1);
			if (end == 1){
				break;
			}
		}
	}
}

void task2(){
	while(1){
		P(0);
		while(1){
			printf("b\n");
			fscanf(com1in, "%s",s1);
			P(1);
			i--;
			V(1);
			if (end == 1){
				break;
			}
		}
	}
}

void task3(){
	while(1){
		i = 0;
		end = 0;
		fprintf(com0out,"ready?\n");
		fprintf(com1out,"ready?\n");
		//timer
		fprintf(com0out, "go!!\n");
		fprintf(com1out, "go!!\n");
		for ( int k = 0; k < 2; k++){
			V(0);	
		}
		while(1){
			if(i > 20){
				//win 0
				end = 1;
				fprintf(com0out,"com0 win!\n");
				fprintf(com1out,"com0 win!\n");
				break;
			}else if(i < -20){
				//win 1
				end = 1;
				fprintf(com0out,"com1 win!\n");
				fprintf(com1out,"com1 win!\n");
				break;
			}	
		}
	}
}
