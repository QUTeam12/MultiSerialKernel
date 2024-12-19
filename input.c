#include "input.h"
extern char inbyte(int port);

void readInput(int port, char *buf){
	char c;
	c = inbyte(port);
	*(buf) = c;
}
