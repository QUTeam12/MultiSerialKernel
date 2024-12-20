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
int trump_index=0;
int dealer_trumps[10];
int dealer_index=0;
int player1_trumps[10];
int player1_index=0;
int player2_trumps[10];
int player2_index=0;

int dealer_score=0;
int player1_score=0;
int player2_score=0;
int player1_double=0;
int player2_double=0;
int dealer_bj=0;
int player1_bj=0;
int player2_bj=0;
int player1_tip=100;
int player2_tip=100;
int player1_bet=0;
int player2_bet=0;
int player1_tip_change=0;
int player2_tip_change=0;

char result_char[1000];

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
int draw_trump(){
	int trump_num=trump[trump_index];
	trump_index++;
	return trump_num;
}


void trump_init(){
	for(int i=1;i<=13;i++){
		for(int j=0;j<4*deck;j++){
			trump[j+(i-1)*4*deck]=i; //iの絵柄のトランプを4*deck枚配置
		}
	}
	srand(get_ms()); // 現在時刻の情報でrand初期化
	printf("time:%u\n",get_ms());
	shuffle(trump,deck*52);
	return;
}


void print_table(FILE *port,int is_result){
	if(is_result){
		fprintf(port,"dealer_trumps:%d,",dealer_trumps[0]);
		for(int i=1;i<10;i++){
			if(dealer_trumps[i]!=0){
				fprintf(port,"%d,",dealer_trumps[i]);
			}
		}
		fprintf(port,"\x8 \n");
	}else{
		fprintf(port,"dealer_trumps:%d\n",dealer_trumps[0]);
	}
	fprintf(port,"player1_trumps:%d,",player1_trumps[0]);
	for(int i=1;i<10;i++){
		if(player1_trumps[i]!=0){
			fprintf(port,"%d,",player1_trumps[i]);
		}
	}
	fprintf(port,"\x8 \n");

	fprintf(port,"player2_trumps:%d,",player2_trumps[0]);
	for(int i=1;i<10;i++){
		if(player2_trumps[i]!=0){
			fprintf(port,"%d,",player2_trumps[i]);
		}
	}
	fprintf(port,"\x8 \n");
	return;
}
int cal_score(int *trumps,int index){
	int score=0;
	int a_num=0;
	for(int i=0;i<index;i++){
		if(trumps[i]==1){
			a_num+=1;
		}else if(trumps[i]==11||trumps[i]==12||trumps[i]==13){
			score+=10;
		}else{
			score+=trumps[i];
		}
	}
	score+=a_num;
	//ここでスコア最小値記録	
	for(int i=0;i<a_num;i++){
		if(score+10<=21){
			score+=10;
			//足せたら、10足して21に近づけるよ
		}
	}
	return score;
}

int dealer_turn(){
	int score=0;
	score=cal_score(dealer_trumps,dealer_index);
	if(score<17){
	for(int i=dealer_index;i<10;i++){
		dealer_trumps[i]=draw_trump();
		dealer_index++;
		score=cal_score(dealer_trumps,dealer_index);
		if(score>=17){
			break;
		}
	}}
	return score;
}
void cal_bj(){
	if(dealer_score==21 && dealer_index==2){
		dealer_bj=1;
	}
	if(player1_score==21 && player1_index==2){
		player1_bj=1;
	}
	if(player2_score==21 && player2_index==2){
		player2_bj=1;
	}
}

void print_results(FILE *port){
	dealer_score=dealer_turn();
	player1_score=cal_score(player1_trumps,player1_index);
	player2_score=cal_score(player2_trumps,player2_index);
	cal_bj();
	//printf("\033[4A\r");  // 4行上に移動して行の先頭へ
	
	fprintf(port,"---------------results---------------------\n");
	print_table(port,1);
	fprintf(port,"--------score--results---------------------\n");
	fprintf(port,"dealer score:%d\n",dealer_score);
	fprintf(port,"player1 score:%d\n",player1_score);
	fprintf(port,"player2 score:%d\n",player2_score);
	fprintf(port,"----------bet--results---------------------\n");
	if(player1_bj>dealer_bj){
		//Black Jack
		fprintf(port,"Player 1: Blackjack! Player1 win 2.5 times your bet!\nPlayer1 won $%d\n",(int)2.5*player1_bet);
		player1_tip+=(int)2.5*player1_bet;
	}else if(player1_bj<dealer_bj){
		fprintf(port, "Dealer: Blackjack! Player1 lose $%d\n",player1_bet);
		player1_tip-=player1_bet;
	}else if((dealer_score>21 && player1_score>21) || dealer_score==player1_score){
		//引き分け
		fprintf(port,"player1 push!\n");
	}else if((dealer_score>21 && player1_score<=21)||(player1_score>dealer_score)){
	       	//プレイヤーの勝ち
		fprintf(port,"Player1 win $%d\n",player1_bet);
		player1_tip+=player1_bet;
	}else{
		//プレイヤーの負け
		fprintf(port,"Player1 lose $%d\n",player1_bet);
		player1_tip-=player1_bet;
	}
}

       			       

void task1(){
	while(1){
		char s0[256];
		char s1[256];
		printf("This is BlackJack Game. Input s to start!\n");
		scanf("%s",s0);
		trump_init();
		printf("your score:$%d\nHow many tips do you want to bet? 1 tip=$10 \n",player1_tip);
		scanf("%d", &player1_bet);
		player1_bet=player1_bet*10;
		printf("player1 bet $%d and player2 bet $%d\n",player1_bet,player2_bet);
		player1_trumps[0]=draw_trump();
		player2_trumps[0]=draw_trump();
		dealer_trumps[0]=draw_trump();
		player1_trumps[1]=draw_trump();
		player2_trumps[1]=draw_trump();
		dealer_trumps[1]=draw_trump();
		player1_index=2;
		player2_index=2;
		int is_stand=0;
		while(! is_stand){
			print_table(com0out,0);
			printf("HIT:type h,STAND:type s,DOUBLE:type d\n");
			scanf("%s",s0);
			if(s0[0] == 'h'){
				if(player1_index>=10){
					printf("no more hit\n");
					is_stand=1;
					break;
				}else{
					player1_trumps[player1_index]=draw_trump();
					player1_index++;
					//printf("\033[4A\r");  // 4行上に移動して行の先頭へ
					//print_table(com0out,0);
				}
			}else if(s0[0] == 'd'){
				if(player1_index>=10){
					printf("no more hit\n");
					is_stand=1;
					break;
				}else{
					player1_trumps[player1_index]=draw_trump();
					//printf("\033[4A\r");  // 4行上に移動して行の先頭へ
					print_table(com0out,0);
					player1_bet=player1_bet*2;
				}
				is_stand=1;
				break;
			}else if(s0[0] == 's'){
				is_stand=1;
			}
		}
		dealer_score=dealer_turn();
		player1_score=cal_score(player1_trumps,player1_index);
		player2_score=cal_score(player2_trumps,player2_index);
		//printf("\033[4A\r");  // 4行上に移動して行の先頭へ
		
		print_results(com0out);
		//printf("\033[3A\r");  // 3行上に移動して行の先頭へ
		//fscanf(com1in, "%s", s1);
		//fprintf(com1out, "p1: %s\n", s1);

		
	}
}


void del_lines(FILE *port,int p){
	fprintf(port,"\r");
	for(int i=0;i<p;i++){
		fprintf(port,"                                                                                                               ");
		fprintf(port,"\033[1A\r");
	}
	fprintf(port,"\n");
}
void task_init(){
	P(1);
	P(2);
	P(3);
	P(4);
}

void p1(){
	FILE *port=com0out;
        char s[256];
        fprintf(port,"This is BlackJack Game. Input something to start!\n");
        scanf("%s",s);
	del_lines(port,2);
	fprintf(port,"Waiting for player2 to start......\n");
	V(2);
	P(1);
	while(1){
		printf("your score:$%d\nHow many tips do you want to bet? 1 tip=$10 \n",player1_tip);
        	scanf("%d", &player1_bet);
        	player1_bet=player1_bet*10;
		V(2);
		P(1);
        	printf("player1 bet $%d and player2 bet $%d\n",player1_bet,player2_bet);
	
	}
}

void p2(){
        FILE *port=com1out;
	FILE *sport=com1in;
        char s[256];
        fprintf(port,"This is BlackJack Game. Input something to start!\n");
        fscanf(sport,"%s",s);
        del_lines(sport,2);
        fprintf(port,"Waiting for player1 to start......\n");
        V(1);
        P(2);
        while(1){
                printf("your score:$%d\nHow many tips do you want to bet? 1 tip=$10 \n",player2_tip);
                scanf(sport,"%d", &player2_bet);
                player2_bet=player2_bet*10;
                V(1);
                P(2);
                printf("player1 bet $%d and player2 bet $%d\n",player1_bet,player2_bet);

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
	set_task(task_init);
	set_task(p1);
	set_task(p2);
	begin_sch();
	return 0;
}
