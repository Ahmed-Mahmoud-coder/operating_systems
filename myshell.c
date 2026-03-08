#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define max 100
char *vars[50];
char *vals[50];
int var_count = 0;


int main (){
    char command[max];

    while(1){
        printf("myshell $ ");

        // Read a full line from the user and remove \n
        fgets(command, max, stdin);
        command[strlen(command)-1] = '\0';  // replace newline with null terminator
            

        // case if user press enter 
        if (strlen(command) == 0) {
            continue;
        }
        // if user write exit it will terminate the shell
        if (strcmp(command, "exit") == 0) {
            break;
        }
        // Split the input line into separate arguments
        char *args[10];
        int i = 0;

        char *token = strtok(command, " ");
        while (token != NULL) 
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        //case 'export VAR=VALUE'
        if (strcmp(args[0], "export") == 0 && args[1] != NULL) {
            char *varname = strtok(args[1], "=");   
            char *varvalue = strtok(NULL, "=");     
    
            if (varname != NULL && varvalue != NULL) 
            {
                vars[var_count] = strdup(varname);   
                vals[var_count] = strdup(varvalue);  
                var_count++;
            }
        continue;
        }
        for (int k = 0; k < i; k++) {       
            if (args[k][0] == '$') {       
                for (int j = 0; j < var_count; j++) {
                    if (strcmp(args[k]+1, vars[j]) == 0) {
                        args[k] = vals[j];  
                        break;
                    }
                }
            }
        }
     
        // case background process
        int background_flag = 0;
        if (i > 0 && strcmp(args[i-1], "&") == 0) 
        {
            background_flag = 1;       // Set background flag
            args[i-1] = NULL;     // Remove '&' from arguments
            }

        // case built-in command 'cd'
        if (strcmp(args[0], "cd") == 0) 
        {
            if (args[1] != NULL) 
            {
                chdir(args[1]);
            } 

            else 
            {
            printf("cd: missing path\n");
            }
            continue; // Skip fork and go back to prompt
        }

        // case echo 
        if (strcmp(args[0], "echo") == 0) 
        {   
            printf("%s\n", args[1]);
            continue;
        }

        // Creates a new process
        pid_t pid = fork();
        // Child process 
        // replace the child process image(parent) with command
        if (pid == 0) 
        {   

            execvp(args[0], args);
            perror("Error");
            exit(1);
        }
        //parent process 
        //parent waits for child to finish before continuing
        else if (pid > 0)
        {   
            //wait only if not background process 
            if (background_flag!=1)
            {
                 wait(NULL);
            }
        }
    }

    return 0;
}
