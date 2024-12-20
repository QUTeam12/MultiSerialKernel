#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>

extern void outbyte(int port, unsigned char c);
extern char inbyte(int port);

void out_msg(int port, const char* msg) { // msgのNULL判定はfprintfインクルード不可なので面倒 
	if (msg == NULL) {
		return;
	}
	while (*msg != '\0') {
		outbyte(port, *msg);
		msg++;
	}
}

int readTask(int port, char* buf, int nbytes) {
    char c;
    int i;
    for (i = 0; i < nbytes; i++) {
        c = inbyte(port);
        switch (c) {
            case '\r':  // CRの場合
            case '\n':  // LFの場合
                outbyte(port, '\r');
                outbyte(port, '\n'); 
                *(buf + i) = '\n';
                return i + 1;
            case '\x1b':  // Escの場合
                out_msg(port, "\r\n\r\n:Command Mode: You have some options.\r\n");
                out_msg(port, "[Enter] Finish editing\r\n");
                out_msg(port, "[w] Restart Write Mode\r\n");
                out_msg(port, "[r] Read Mode\r\n");
                out_msg(port, "#Attention# The user in Write mode is only one.\r\n\r\n");
                *(buf + i) = '\v';
                return i + 1;
            case '\x7f':  // バックスペースの場合
                if (i > 0) {
                    outbyte(port, '\x8');
                    outbyte(port, ' ');
                    outbyte(port, '\x8');
                    i--;
                }
                i--;
                continue;
            default:  // 通常文字の場合
                outbyte(port, c);
                *(buf + i) = c;
                break;
        }
    }
    return i;
}

int read(int fd, char* buf, int nbytes) {
    int i = 0;
    switch (fd) {
        case 0:
        case 3:
            i = readTask(0, buf, nbytes);
            break;
        case 4:
            i = readTask(1, buf, nbytes);
            break;
        default:
            return EBADF;
    }
	return i;
}


void writeTask(int port, char* buf, int nbytes) {
    int i, j;
    for (i = 0; i < nbytes; i++) {
        if (*(buf + i) == '\n') {
            outbyte(port, '\r'); /* LF -> CRLF */
        }
        outbyte(port, *(buf + i));
        for (j = 0; j < 300; j++);
    }
}

int write(int fd, char* buf, int nbytes) {
    switch (fd) {
        case 1:
        case 2:
        case 3:
            writeTask(0, buf, nbytes);
            break;
        case 4:
            writeTask(1, buf, nbytes);
            break;
        default:
            return EBADF;
    }
    return nbytes;
}


int fcntl(int fd, int cmd, ...) {
    if (cmd == F_GETFL) {
        return O_RDWR;
    } else {
        return 0;
    }
}
