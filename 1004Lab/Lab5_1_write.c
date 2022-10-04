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
void writer();

int main()
{
    writer();
    return 0;
}
void writer()
{
    int fd = 0;
    Contents con[3] = {
        {1.1, "Test1", 1},
        {2.2, "Test2", 2},
        {3.3, "Test3", 3}};
    
    if ((fd = open("./Test", O_WRONLY | O_CREAT | O_TRUNC, 0644))== -1)
    {
        perror("failed open file.");
        exit(1);
    }

    if (write(fd, con, sizeof(con)) == -1)
    {
        perror("failed write ");
        exit(1);
    }

    close(fd);
}