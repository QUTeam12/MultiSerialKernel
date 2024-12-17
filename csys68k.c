#include <fcntl.h>
// #include <varargs.h>

extern void outbyte(int fd, unsigned char c);
extern char inbyte(int fd);

/*
int fcntl(int fd, int cmd, ...) {
	if (cmd == F_GETFL) {
		return O_RDWR;
	} else {
		return 0;
}
*/

int read(int fd, char *buf, int nbytes)
{
  char c;
  int  i;

  for (i = 0; i < nbytes; i++) {
    c = inbyte(0);

    if (c == '\r' || c == '\n'){ /* CR -> CRLF */
      outbyte(0, '\r');
      outbyte(0, '\n');
      *(buf + i) = '\n';

    /* } else if (c == '\x8'){ */     /* backspace \x8 */
    } else if (c == '\x7f'){      /* backspace \x8 -> \x7f (by terminal config.) */
      if (i > 0){
	outbyte(0, '\x8'); /* bs  */
	outbyte(0, ' ');   /* spc */
	outbyte(0, '\x8'); /* bs  */
	i--;
      }
      i--;
      continue;

    } else {
      outbyte(0, c);
      *(buf + i) = c;
    }

    if (*(buf + i) == '\n'){
      return (i + 1);
    }
  }
  return (i);
}

int write (int fd, char *buf, int nbytes)
{
  int i, j;
  for (i = 0; i < nbytes; i++) {
    if (*(buf + i) == '\n') {
      outbyte (0, '\r');          /* LF -> CRLF */
    }
    outbyte (0, *(buf + i));
    for (j = 0; j < 300; j++);
  }
  return (nbytes);
}
