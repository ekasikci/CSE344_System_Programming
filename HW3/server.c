#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

sem_t semaphore;

// implements the commands sent by the clients
char* client_requests(FILE* logfile, char* request, int server_pid, int client_pid, int num_of_client) {
    char* first_command = strtok(request, " \t\n");

    if(strcmp(first_command, "help") == 0) return "Available comments are:\n\nhelp, list, readF, writeT, upload, download, quit, killServer\n\n";
    else if (strcmp(first_command, "list") == 0) system("ls"); 
    else if (strcmp(first_command, "readF") == 0) {
        char*  filename = strtok(NULL, " \t\n");
        int line = atoi(strtok(NULL, " \t\n"));

        FILE* file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }

        if (line > 0) {
            // Display the line
            char line_content[1024];
            int currentLine = 1;
            while (fgets(line_content, sizeof(line_content), file)) {
                if (currentLine == line) {
                    printf("Line %d: %s", line, line_content);
                    break;
                }
                currentLine++;
            }
        } else {
            // Display the entire contents of the file
            char character;
            while ((character = fgetc(file)) != EOF) {
                putchar(character);
            }
        }

        fclose(file);
    }
    else if (strcmp(first_command, "writeF") == 0) {
        char*  filename = strtok(NULL, " \t\n");
        int line = atoi(strtok(NULL, " \t\n"));

    }
    else if (strcmp(first_command, "upload") == 0) {
        char*  filename = strtok(NULL, " \t\n");

    }
    else if(strcmp(first_command, "download") == 0) {
        char*  filename = strtok(NULL, " \t\n");

    }
    else if(strcmp(first_command, "quit") == 0) {
        return "Sending write request to server log file\n""waiting for logfile ...\n";
        fprintf(logfile, "Client PID: %d requested to quit\n", client_pid);  
        fclose(logfile);
        kill(client_pid, SIGKILL); 
        return "logfile write request granted\nbye..\n";

    }
    else if(strcmp(first_command, "killServer") == 0) {     
        printf(">> kill signal from client%d..terminating...\n", num_of_client);  
        fprintf(logfile, "Client PID: %d killed the Server\n", client_pid);  
        fclose(logfile);
        kill(server_pid, SIGKILL); 
        printf("bye\n");
    }
    else return "Invalid Command!\n";
        
}

int start_server(char *directory_name, int max_num_of_clients) {
    int result = mkdir(directory_name, 0777);  // 0777 gives read, write, and execute permissions to the owner, group, and others

    if (result == 0)
        printf("Directory created successfully.\n");
    else 
        printf("Directory already exist.\n");

    strcat(directory_name, "/log");
    FILE* logfile = fopen(directory_name, "a+");
    
    printf("biboServer Here %d\n\n", max_num_of_clients);
    char filename[20];

    // Server PID
    pid_t server_pid = getpid();
    sprintf(filename, "%d", server_pid);
    printf("Server Started PID %d...\n", server_pid);

    if (mkfifo (filename, 0777) == -1)
        printf("Couldn't create FIFO");

    printf("Waiting for clients...\n");
    int fd = open(filename, O_RDONLY);

    if (fd == -1)
        perror("Error opening System FIFO for reading");
    
    char buffer[255];
    int num_of_client = 0;

    sem_init(&semaphore, 0, max_num_of_clients);
    // Continuously read from the FIFO
    while (1) {
        // Read data from the FIFO
        int bytesRead = read(fd, buffer, 255);

        if (bytesRead == -1) {
            perror("Error reading from FIFO");
            break;
        }
        else if (bytesRead > 0) {
            // Null-terminate the buffer
            // buffer[bytesRead] = '\0';

            char* connection_type = strtok(buffer, " \t\n");
            char* client_pid = strtok(NULL, " \t\n");
            int client_pid_int = atoi(client_pid);

            // If the client sent Connect request
            if((strcmp(connection_type, "Connect") == 0)) {
                sem_wait(&semaphore);
                
                int pid = fork();
                num_of_client++;
                if (pid == 0){
                    char client_pid_file[10];
                    strcpy(client_pid_file, client_pid);
                    strcat(client_pid_file, "_w");
                    int fd_r = open(client_pid_file, O_RDONLY);
                    client_pid_file[strlen(client_pid_file) - 1 ] = 'r';
                    sleep(1); // waits for the client to open the second FIFO
                    int fd_w = open(client_pid_file, O_WRONLY);
                    if (fd_r == -1 || fd_w == -1) 
                        printf("Couldn't connect to the server!\n");
                    else {
                        printf("Client PID %s connected as client%d\n", client_pid, num_of_client);
                        fprintf(logfile, "Client PID %s connected", client_pid);
                        while (1) {
                            sleep(0.2);
                            int bytesRead = read(fd_r, buffer, 255);

                            if (bytesRead == -1) {
                                perror("Error reaing from FIFO");
                                break;
                            }
                            if (bytesRead > 0) write(fd_w, client_requests(logfile, buffer, server_pid, client_pid_int, num_of_client), 255);

                            if (kill(client_pid_int, 0) != 0) break;                          
                        }
                    }
                    // close FIFO's
                    close(fd_w);
                    close(fd_r);
                    printf("client%d disconnected\n", client_pid_int);
                    sem_post(&semaphore);
                }
            }
            // If the client sent tryConnect request
            if((strcmp(connection_type, "tryConnect") == 0)) {
                // If there is a place for the client
                if((num_of_client < max_num_of_clients)) {
                    sem_wait(&semaphore);
                    int pid = fork();
                    num_of_client++;
                    if (pid == 0){
                        strcat(client_pid, "_w");
                        int fd_r = open(client_pid, O_RDONLY);
                        client_pid[strlen(client_pid) - 1 ] = 'r';
                        sleep(1); // waits for the client to open the second FIFO
                        int fd_w = open(client_pid, O_WRONLY);
                        if (fd_r == -1 || fd_w == -1) 
                            printf("Couldn't connect to the server!\n");
                        else {
                            printf("Client PID %s connected as client%d\n", client_pid, num_of_client);
                            printf("Client PID %s connected", client_pid);
                            int client_pid_int = atoi(client_pid);
                            while (1) {
                                printf("f\n");
                                sleep(0.2);
                                int bytesRead = read(fd_r, buffer, 255);

                                if (bytesRead == -1) {
                                    perror("Error reaing from FIFO");
                                    break;
                                }
                                if (bytesRead > 0) {
                                    printf("%s\n", buffer);
                                    write(fd_w, client_requests(logfile, buffer, server_pid, client_pid_int, num_of_client), 255);
                                } 
                            }
                        }
                        // close FIFO's
                        close(fd_w);
                        close(fd_r);
                        sem_post(&semaphore);
                    }
                }
                // If there is not a place for the client
                else printf("Server is full! Try to connect later./n/n");
            }    
        }     
    }
    sem_destroy(&semaphore);
}
 

int main(int argc, char *argv[]) {

    printf("Select Side: Create a server using following command\n"
           "biboServer <directory name> <max number of clients>\n"
           "Input: ");

    char input[100];

    // Get an input from the user to start the biboServer
    fgets(input, sizeof(input), stdin);

    char* user = strtok(input, " \t\n");

    
    if(strcmp(user, "biboServer") == 0) {
        char*  dirname = strtok(NULL, " \t\n");
        int max_num_of_clients = atoi(strtok(NULL, " \t\n"));

        // Start biboServer
        if(strcmp(user, "biboServer") == 0)
            start_server(dirname, max_num_of_clients);
    }
    else {
        printf("Invalid Input!\n");
        return 1;
    }

    return 0;
}