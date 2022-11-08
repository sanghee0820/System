#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
	int fd1, fd2, nread;
	char msg[100];

	fd1 = open ("fifo1", O_WRONLY);
	fd2 = open ("fifo2", O_RDWR);

	while(1) {
		printf ("Enter a string to send : ");
		fgets (msg, sizeof(msg), stdin);

		write (fd1, msg, strlen(msg) + 1);

		if(!strcmp(msg, "end\n")) {
			break;
		}

		nread = read (fd2, msg, 100);
		printf ("%s\n", msg);
	}
	
	close(fd1);
	close(fd2);
}

