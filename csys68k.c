#include <fcntl.h>
#include <stdarg.h>

extern void outbyte(int port, unsigned char c);
extern char inbyte(int port);

int read_line(int port, char* buf, int nbytes) {
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
            case '\x7f':  // バックスペースの場合
                if (i > 0) {
                    outbyte(port, '\x8');
                    outbyte(port, ' ');
                    outbyte(port, '\x8');
                    i--;
                }
                i--;
                break;
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
            i = read_line(0, buf, nbytes);
            break;
        case 4:
            i = read_line(1, buf, nbytes);
            break;
        default:
            return EBADF;
    }
    return i;
}

void write_task(int port, char* buf, int nbytes) {
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
            write_task(0, buf, nbytes);
            break;
        case 4:
            write_task(1, buf, nbytes);
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
