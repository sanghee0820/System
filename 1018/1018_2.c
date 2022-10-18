#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int usr_pid = 0;

void my_sig_handler(int signum){
    
    while(usr_pid == 0) sleep(1);
    kill(usr_pid, SIGUSR1);
    printf("SIGUSR2 Catch !! Program End\n");
    exit(0);
}
int main (void)
{
	pid_t pid;
	pid = getpid();

    signal(SIGUSR2, my_sig_handler);

	printf("My pid = %d \n", pid);
    printf("Input other's pid : ");
    scanf("%d", &usr_pid);


    while(1){
        sleep(10);
    }
}