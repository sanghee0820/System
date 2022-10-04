#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>

int main(int argc, char* argv[]){
	int fd_in, fd_out;
	char* Data = (char*)malloc(sizeof(char)*1024);

	fd_in = open(argv[1], O_RDWR|O_CREAT,S_IWUSR|S_IRUSR);
	fd_out = open(argv[2],O_RDWR|O_CREAT,S_IWUSR|S_IRUSR);	
	
	read(fd_in, Data,1024);
	write(fd_out,Data,strlen(Data));

	close(fd_in);
	close(fd_out);
	free(Data);

	return 0;
}
