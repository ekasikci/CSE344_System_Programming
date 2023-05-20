#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

int q2(int fd){
    int dupfd, dup2fd;
    
    // firstly checks if fd is valid, if it is not then returns -1
    if(fcntl(fd, F_GETFL) == -1)
    {
        errno = EBADF;
        perror("error: ");
        close(dupfd);
        return -1;
    }
    
    dupfd = dup(fd);
    // for dup 2 if oldfd and newfd are equal than it cheks if oldfd descriptor valid by using fnctl function(it returns -1 if it is not valid)
    if (dupfd == fd+1 && fcntl(dupfd, F_GETFL) == -1)
    {
        errno = EBADF;
        perror("error: ");
        close(dupfd);
        return -1;
    }
    else
        dup2fd = dup2(dupfd, fd+1);

    printf("Question 2\n\n --------------------------------------------------\n");
    printf("fd: %d\n", fd);
    printf("dupfd : %d\n", dupfd);
    printf("dup2fd: %d\n", dup2fd);
    printf("--------------------------------------------------\n\n\n");

    close(dupfd);
    close(dup2fd);

    return 1;
}
