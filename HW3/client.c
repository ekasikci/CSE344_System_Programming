#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

int connect_server(char* connection_type, int server_pid) {

    char filename[20];
    sprintf(filename, "%d", server_pid);

    int fd = open(filename, O_WRONLY); // Connect Server FIFO

    if (fd == -1) {
        printf("Couldn't connect to the server!\n");
        return -1;
    }

    pid_t pid = getpid();

    sprintf(filename, "%d", pid);

    strcat(connection_type, " ");
    strcat(connection_type, filename);
    
    // Create client FIFO and send connection type and PID of the client to the server FIFO
    strcat(filename, "_w");
    if (mkfifo (filename, 0777) == -1)
        printf("Couldn't create writing FIFO");
    write(fd, connection_type, 20);
    int fd_w = open(filename, O_WRONLY);
    filename[strlen(filename) - 1 ] = 'r';
    if (mkfifo (filename, 0777) == -1)
        printf("Couldn't create reading FIFO");
    int fd_r = open(filename, O_RDONLY);

    if (fd_r == -1 || fd_w == -1) perror("Error opening FIFO's");

    char buffer[255];
    // Continuously write to the client FIFO
    while (1) {
        printf("Command for the server: ");
        scanf("%s", buffer);
        write(fd_w, buffer, 255);
        sleep(0.2); // Waits an answer from the server
        int bytesRead = read(fd_r, buffer, 255);
        if (bytesRead == -1) {
            perror("Error reaing from FIFO");
            break;
        }
        if (bytesRead > 0) printf("%s", buffer);
            
    }

    // close FIFO's
    close(fd_w);
    close(fd_r);
}
int main(){

    printf("Clinent Side: Connect to a server using the following command\n"
           "biboClient <Connect/tryConnect> <ServerPID>\n"
           "Input: ");

    char input[100];

    // Get an input from the user to start the biboServer
    fgets(input, sizeof(input), stdin);

    char* user = strtok(input, " \t\n");

    if(strcmp(user, "biboClient") == 0) {
        char* connection_type = strtok(NULL, " \t\n");
        if(!(strcmp(connection_type, "Connect") || strcmp(connection_type, "tryConnect")))
        {
            printf("Invalid Input!\n");
            return 2;
        }
        int server_pid = atoi(strtok(NULL, " \t\n"));

        connect_server(connection_type, server_pid);
    }
    else {
        printf("Invalid Input!\n");
        return 1;
    }

    return 0;
}