#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int usr_pid = 0;

void my_sig_handler(int signum){
    while(usr_pid == 0) sleep(1);
    kill(usr_pid, SIGUSR2);
    printf("SIGUSR1 Catch !! Program End\n");
    exit(0);
}
int main (void)
{

    signal(SIGUSR1, my_sig_handler);
	pid_t pid;
	pid = getpid();

	printf("My pid = %d \n", pid);

    printf("Input other's pid : ");
    scanf("%d", &usr_pid);


    while(1){
        sleep(1);
    }
}