#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int main(int argc, char *argv[])
{
    if(argc!=2) {
        printf("Usage: %s <serial port>\n", argv[0]);
        return 1;
    }

    int blinkyfd = open(argv[1], O_RDWR | O_NOCTTY | O_SYNC );
    if(blinkyfd == -1) {
        perror("open");
        return 1;
    }

    close(blinkyfd);

    return 0;
}

