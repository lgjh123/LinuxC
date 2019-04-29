#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main()
{
    int fd = 0;
    fd = open("hello",2);
    printf("123\n");
    close(fd);
    return 0;
}
