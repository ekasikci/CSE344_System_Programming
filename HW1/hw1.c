#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "hw1q2.c"
#include "hw1q3.c"


int main(int argc, char *argv[])
{
    // unused int descriptor value was selected purposely, otherwise it would be 0 and even if any valid value wouldn't be assigned to it we couldn't check if it is valid or not
    int fd = 15;
    
    // write only and create file flags are selected
    int flags = O_WRONLY | O_CREAT;

    // 0th argument -> file execution 
    // 1st argument -> file name 
    // 2nd argument -> number of bytes
    // 3rd argument -> x letter (arbitrary)
    // If the 3rd command line argumnet x is omitted than case 3 executes. If x was typed than case 4 executes. In other cases program warns the user 
    // that command line arguments are invalid.
    if(argc == 3){
        int numofBytes = atoi(argv[2]);
        flags |= O_APPEND; // adds O_APPEND flag if 'x' is omitted

        fd = open(argv[1], flags, S_IRUSR | S_IWUSR);
        if (fd == -1) 
        {
            perror("open error");
            exit(1);
        }

        for(int i = 0; i < numofBytes; i++)
            write(fd, "o", 1);
    }
    else if(argc == 4){
        int numofBytes = atoi(argv[2]);
        if(strcmp(argv[3], "x") != 0){
            printf("Unexpected command-line arguments!\n");\
            exit(1);
        }

        fd = open(argv[1], flags, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            perror("open error");
            exit(1);
        }

        for(int i = 0; i < numofBytes; i++){
            lseek(fd, 0, SEEK_END);
            write(fd, "x", 1);
        } 
    }
    else
        printf("Unexpected command-line arguments!\n");

    // returns -1 if fd is not valid
    int q2_val = q2(fd);


    // if fd is valid than checks if fd and dupfd shares the same file offset
    if(q2_val != -1)
    {
        int dupfd = dup(fd);
        if(q3(fd, dupfd))
            printf("Question 3: fd and dupfd (which is fd value of dup(fd) shares the same offset\n");
        else
            printf("Question 3: fd and dupfd (which is fd value of dup(fd) doesn't share the same offset\n");
    }
    close(fd); 
}