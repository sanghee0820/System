#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

int sig_cnt = 0;

void my_sig_handler (int signum)
{   
    sig_cnt++;
	struct tm tm;
        // time(NULL) = current time
    time_t ct = time(NULL);
    tm = *localtime(&ct);   
    ct = time(NULL);
    tm = *localtime(&ct);
    printf("Time ->  Hour : %d ,  Min   : %d ,   Sec  :   %d \n", tm.tm_hour, tm.tm_min, tm.tm_sec);
    printf("LeeSangHee\n\n");
	alarm (5);
    if(sig_cnt == 3) exit(0);
}



int main ()
{
    /* print after 5sec, and printed 3 times EXIT */
	signal (SIGALRM, my_sig_handler);
	alarm (5);
	while (1) {
		sleep (20);
	}
}