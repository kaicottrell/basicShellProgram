#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    printf("Welcome to the most basic shell program ever!\n");

    bool runTerminal = true;
    char command[32];
    char hostname[64];
    char currentWD[256];
    
    getcwd(currentWD, sizeof(currentWD)); // get the initial current working directory
    
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        hostname[0] = '\0'; // blank string
    } 

    while (runTerminal) {
        printf("%s--%s>", hostname, currentWD); // print the current working directory
        fgets(command, sizeof(command), stdin);
        
        // Remove trailing newline character
        command[strcspn(command, "\n")] = '\0';
        // if the command is built in
        if (strcmp(command, "quit") == 0) {
            runTerminal = false;
            printf("Terminal Session Ended.");
        } else if (strncmp(command, "cd", 2) == 0) {
            char directory[256]; // directory user wants to change to
            strcpy(directory, command + 3); // parse out the "cd " to get the directory
            directory[sizeof(directory) - 1] = '\0'; // ensure null-termination

            if (strcmp(command, "cd") == 0) {
                chdir("/"); // go to home directory
                getcwd(currentWD, sizeof(currentWD));
            } else if (chdir(directory) != 0) { // changes the directory here if it is valid
                printf("Failed to change directory.\n");
            } else {
                getcwd(currentWD, sizeof(currentWD)); // update the current working directory
            }
        // if the command is external
        } else {
            pid_t pid;
            pid = fork(); // create a child process

            if (pid == 0) { // child process
                char* args[32]; 
                char* tkn = strtok(command, " ");
                int i = 0;

                while (tkn != NULL) {
                    args[i] = tkn;
                    tkn = strtok(NULL, " ");
                    i++;
                }

                args[i] = NULL; 

                execvp(args[0], args); // first parameter is the command needed to be run and the second paramter is the commands parameter(s)
                perror("execvp"); // if execvp encounters an error, it returns
                return 1;
            } else if (pid < 0) { // error forking
                printf("Error forking");
            } else { // parent process
                int status;
                waitpid(pid, &status, 0); // parent waits for child process to complete
                continue; // restart the loop
            }
        }
    }

    return 0;
}
