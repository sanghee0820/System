#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
        int fd1, fd2, nread;
        char msg[100];

        fd1 = open ("fifo1", O_RDWR);
        fd2 = open ("fifo2", O_WRONLY);

        while(1) {
            nread = read (fd1, msg, 100);

            if(!strcmp(msg, "end\n")) {
			    break;
		    }

            printf ("Received : %s", msg);

            msg[strlen(msg)-1]=' ';
            strcat(msg, "----> SangHee Received");

            write (fd2, msg, strlen(msg) + 1);
        }

        close(fd1);
        close(fd2);
}
