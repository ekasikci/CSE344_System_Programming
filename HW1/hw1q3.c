#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

// Cheks if to fd values share the same offset
bool q3(int oldfd, int newfd){

    struct stat stat1, stat2;
    if(fstat(oldfd, &stat1) < 0 || fstat(newfd, &stat2) < 0) return false;

    // lseek returns the current offset position of that file. If ther are the same for oldfd and newfd the can be same.
    // But to be sure st_dev and st_ino values are compared to see if they are same.
    if((lseek(oldfd, 0, SEEK_CUR) == lseek(newfd, 0, SEEK_CUR)) && 
    ((stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino)))
        return true;
    
    return false;
}
