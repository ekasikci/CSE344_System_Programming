#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_LEN 1000
#define MAX_COMMANDS 20 // Accepts up to 20 commands

// Creates log files for child processes
void createLogFile(char* command){
    time_t current_time = time(NULL);
    char file_name[50];
    sprintf(file_name, "%ld", current_time);
    FILE* log_file = fopen(file_name, "w");
    fprintf(log_file, "Command: %s\n", command);
    fclose(log_file);
}


int main() {
    char input[MAX_LEN];
    printf("Enter a string: ");
    fgets(input, MAX_LEN, stdin); // Gets the string
    input[strcspn(input, "\n")] = 0;
    char *commands[MAX_COMMANDS];
    int fd[2];
    if(pipe(fd) == -1)
        return 1;
    
    char *token = strtok(input, "|");
    int i = 0;
    while(token != NULL && i < MAX_COMMANDS) {
        commands[i++] = token;
        token = strtok(NULL, "|");
    }
    
    for(int j = 0; j < i; j++) {

        // Program terminates if it encounters with ":q" command
        if(strcmp(commands[j], ":q") == 0 ||
        strcmp(commands[j], " :q") == 0  ||
        strcmp(commands[j], ":q ") == 0 )
            exit(0);

            int pid1 = fork();

            if(pid1 < 0)
                return 2;

            // child process
            if (pid1 == 0){
                dup2(fd[1], STDOUT_FILENO); // binds enter of the pipe with std out 
                close(fd[0]);
                close(fd[1]);

                createLogFile(commands[j]);

                execl("/bin/sh", "sh", "-c", commands[j], NULL); 
            }

            int pid2 = fork();

            if(pid2 < 0)
                return 3;

            // child process
            if (pid2 == 0){
                dup2(fd[0], STDIN_FILENO); // binds exit of the pipe with std in 
                close(fd[0]);
                close(fd[1]);

                createLogFile(commands[j+1]);

                execl("/bin/sh", "sh", "-c", commands[j+1], NULL);
            }
            close(fd[0]);
            close(fd[1]);

            wait(NULL);
            j++;

    }

    return 0;
}


