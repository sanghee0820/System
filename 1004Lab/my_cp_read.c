#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
typedef struct Contents
{
    double doublenum;
    char str[12];
    int number;
} Contents;
void reader();

int main()
{
    reader();
    return 0;
}

void reader()
{
    int fd = 0;
    Contents structure;
    if ((fd = open("Test", O_RDONLY,S_IWUSR|S_IRUSR) )== -1)
    {
        perror("failed open file.");
        exit(1);
    }
    int re = 0;
    while ((re = read(fd, &structure, sizeof(Contents))) > 0)
    {
        printf("Double:%-10lf", structure.doublenum);
        printf("\tString:%-20s", structure.str);
        printf("\tnumber:%-10d\n", structure.number);
    }
    close(fd);
}