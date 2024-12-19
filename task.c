#include "task.h"
#include "mtk_c.h"
#include "input.h"
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
volatile int login_com0 = 0;
volatile int login_com1 = 0;
volatile int end = 0;

void task1(){
	char s0 = 'a';
	while(1){
		fprintf(com0out,"準備ができたら ENTER\n");
		while(1){
			readInput(0,&s0);
			if(s0 == '\r') break;
		}
		fprintf(com0out, "ready\n");
		login_com0 =1;
		P(0);
		while(1){
			while(1){
				readInput(0,&s0);
				if(s0 == '\r') break;
			}
			if (end == 1){
				break;
			}
			P(1);
			i++;
			V(1);
			}
		}
	}
void task2(){
	char s1 = 'a';
	while(1){
		fprintf(com1out,"準備ができたら ENTER\n");
		while(1){
			readInput(1,&s1);
			if(s1 == '\r') break;
		}
		fprintf(com1out, "ready\n");
		login_com1 =1;
		P(0);
		while(1){
			while(1){
				readInput(1,&s1);
				if(s1 == '\r') break;
			}
			if (end == 1){
				break;
			}
			P(1);
			i--;
			V(1);
		}
	}
}

void task3(){
	double start_time , current_time;
	while(1){
		while(1){
			if(login_com0 && login_com1){
				break;
			}
		}
		login_com0 = 0;
		login_com1 = 0;
		start_time = 0;
		current_time = 0;
		i = 0;
		end = 0;
		fprintf(com0out,"ready?\n");
		fprintf(com1out,"ready?\n");
		while(1){
			current_time += 0.5f;
			if((current_time - start_time) > 2000){
				//timer
				fprintf(com0out, "go!!\n");
				fprintf(com1out, "go!!\n");
				break;
			}
		}
		for ( int k = 0; k < 2; k++){
			V(0);	
		}
		while(1){
			fprintf(com0out,"\x8\x8  ");
			fprintf(com0out,"\r");
			fprintf(com1out,"\x8\x8  ");
			fprintf(com1out,"\r");
			fprintf(com0out,"%d",i);
			fprintf(com1out,"%d",i);
			if(i >= 20){
				//win 0
				end = 1;
				fprintf(com0out,"com0 win!\n");
				fprintf(com1out,"com0 win!\n");
				break;
			}else if(i <= -20){
				//win 1
				end = 1;
				fprintf(com0out,"com1 win!\n");
				fprintf(com1out,"com1 win!\n");
				break;
			}	
		}
		continue;
	}
}
