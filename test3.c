#define TEST2
#include "mtk_c.h"
#include <stdio.h>
#include <stdlib.h>
FILE *com0in; /* UART1からの読み込み*/
FILE *com0out;/* UART1からの書き込み*/
FILE *com1in;/* UART2からの読み込み*/
FILE *com1out;/* UART2からの書き込み*/
#define deck 2
int trump[deck*52];


/* min_valからmax_valの範囲で整数の乱数を返す関数 */
int get_rand(int min_val, int max_val) {
    return (int)((rand() % (max_val+1 - min_val)) + min_val);
}


/* 要素数sizeの配列arrayの要素をシャッフルする関数 */
void shuffle(int* array, int size) {
    for (int i=0; i < size; i++) {
        int r = get_rand(i, size-1);
        int tmp = array[i];
        array[i] = array[r];
        array[r] = tmp;
    }
}

void trump_init(){
	for(int i=1;i<=13;i++){
		for(int j=0;j<4*deck;j++){
		       trump[j+(i-1)*4*deck]=i;
		}
	}
	srand(get_ms()); // 現在時刻の情報でrand初期化
	printf("time:%u\n",get_ms());
	shuffle(trump,deck*52);
}

void task1(){
	while(1){
		char s0[256];
		char s1[256];
		printf("This is BlackJack Game. Input s to start!\n");
		scanf("%s",s0);
		trump_init();
    		for(int i = 0; i < 3; i++) {
        		printf("%d\n",trump[i]);
    		}
		//fscanf(com1in, "%s", s1);
		//fprintf(com1out, "p1: %s\n", s1);
		
	}
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
