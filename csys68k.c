#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>

extern void outbyte(int port,unsigned char c);
extern char inbyte(int port);

int readTask(int port, char *buf,int nbytes){
  char c;
  int  i;

  for (i = 0; i < nbytes; i++) {
    c = inbyte(port);

    if (c == '\r' || c == '\n'){ /* CR -> CRLF */
      outbyte(port,'\r');
      outbyte(port,'\n');
      *(buf + i) = '\n';

    /* } else if (c == '\x8'){ */     /* backspace \x8 */
    } else if (c == '\x7f'){      /* backspace \x8 -> \x7f (by terminal config.) */
      if (i > 0){
	outbyte(port,'\x8'); /* bs  */
	outbyte(port,' ');   /* spc */
	outbyte(port,'\x8'); /* bs  */
	i--;
      }
      i--;
      continue;

    } else {
      outbyte(port,c);
      *(buf + i) = c;
    }

    if (*(buf + i) == '\n'){
      return (i + 1);
    }
  }
  return i;
}

int read(int fd, char *buf, int nbytes)
{
  int i = 0;
  if(fd == 0 || fd == 3){
    i = readTask(0,buf,nbytes);
  }else if (fd == 4){
    i = readTask(1,buf,nbytes);  
  }else{
    return EBADF;
  }
  return (i);
}

void writeTask(int port, char *buf, int nbytes){
  int i, j;
  for (i = 0; i < nbytes; i++) {
    if (*(buf + i) == '\n') {
      outbyte (port,'\r');          /* LF -> CRLF */
    }
    outbyte (port,*(buf + i));
    for (j = 0; j < 300; j++);
  }
}


int write (int fd, char *buf, int nbytes)
{
  if(fd == 1 || fd == 2 || fd == 3){
    writeTask(0,buf,nbytes);
  }else if(fd == 4){
    writeTask(1,buf,nbytes);
  }else{
    return EBADF;
  }
  return (nbytes);
}
int fcntl(int fd, int cmd, ...){
	if(cmd == F_GETFL) {
		return O_RDWR;
	}else{
		return 0;
	}	
}
