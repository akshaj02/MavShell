// Ak
// 1001

// The MIT License (MIT)
//
// Copyright (c) 2016, 2017, 2020 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n" // We want to split our command line up into tokens
                           // so we need to define what delimits our tokens.
                           // In this case  white space
                           // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11 // Mav shell only supports ten arguments

int main()
{

    char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);

    while (1)
    {
        // Print out the msh prompt
        printf("msh> ");

        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
        while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin))
            ;

        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];

        int token_count = 0;

        // Pointer to point to the token
        // parsed by strsep
        char *argument_ptr;

        char *working_str = strdup(cmd_str);

        // we are going to move the working_str pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *working_root = working_str;

        // Tokenize the input stringswith whitespace used as the delimiter
        while (((argument_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
               (token_count < MAX_NUM_ARGUMENTS))
        {
            token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
            if (strlen(token[token_count]) == 0)
            {
                token[token_count] = NULL;
            }
            token_count++;
        }

        // boolean to check if the command was found
        FILE *fp;
        fp = fopen("/home/cse3320/history.txt", "a"); //open a file called history.txt
        fprintf(fp, "%s", cmd_str); // this will print every line typed 
                                    //to a text file to be read later
        fclose(fp);

        int found = 0; // integer boolean to check if the command was found or not
                       // 0 = true, 1 = false

        // if the user presses enter, do nothing and add a new line
        if (token[0] == NULL)
        {
            main();
        }

        // if the user types exit, exit the shell else continue
        else if (strcmp(token[0], "exit") == 0)
        {
            remove("/home/cse3320/history.txt"); // remove the history.txt file
            remove("/home/cse3320/pid.txt"); // remove the pid.txt file
            free(working_root); // free the memory
            exit(0);          // exit the shell
        }

        //if the user types cd, change the directory
        else if (strcmp(token[0], "cd") == 0)
        {
            FILE *ptr;                                  //
            // Opening file in reading mode             //  
            ptr = fopen("/home/cse3320/pid.txt", "a");  //
            // store the pid in a variable using fscanf // This part opens a file called pid.txt 
            int pid;                                    // and stores the pid in a variable
            pid = getpid();                             // which is then printed to the file

            if (pid != 0)                               //      
            {
                fprintf(ptr, "%d\n", pid);
            }

            fclose(ptr);
            chdir(token[1]);
            found = 0; // set found to 0 because the command was found
        }

        // if the user types history, print out the the last 10 commands
        else if (strcmp(token[0], "history") == 0)
        {
            FILE *ptr2;
            // Opening file in reading mode
            ptr2 = fopen("/home/cse3320/pid.txt", "a");   // pid.txt to store the pid
            // store the pid in a variable using fscanf
            int pid;
            pid = getpid();

            if (pid != 0)
            {
                fprintf(ptr2, "%d\n", pid);
            }

            fclose(ptr2);
            FILE *ptr;
            // Opening file in reading mode
            ptr = fopen("/home/cse3320/history.txt", "r"); // history.txt to read the history

            char line[MAX_COMMAND_SIZE][MAX_COMMAND_SIZE];
            int i = 0;

            // read the file line by line and store it in a 2d array
            while (fgets(line[i], MAX_COMMAND_SIZE, ptr) != NULL)
            {
                line[i][strlen(line[i]) - 1] = '\0'; //add a null terminator to the end of the line
                i++;
            }

            // find the length of the array
            int length;

            //finding the length of the array
            for (length = 0; length < MAX_COMMAND_SIZE; length++)
            {
                if (line[length][0] == '\0')
                {
                    break;
                }
            }

            int lineNumber = 0;
            if (length < 15) //if the length is less than 15, print the entire array
            {
                for (int j = 0; j < length; j++)
                {
                    printf("%d: %s\n", lineNumber, line[j]);
                    lineNumber++;
                }
            }
            else //if the length is greater, print the last 15 lines
            {
                for (int j = length - 15; j < length; j++)
                {
                    printf("%d: %s\n", lineNumber, line[j]);
                    lineNumber++;
                }
            }
            found = 0; //set found to 0 becuase the command was found
            // Closing the file
            fclose(ptr);
        }

        //if the user types listpids, show the pids of the last 15 processes
        else if (strcmp(token[0], "listpids") == 0)
        {

            FILE *ptr;
            // Opening file in reading mode
            ptr = fopen("/home/cse3320/pid.txt", "r"); //open the pid.txt file to read the pids
            char line[MAX_COMMAND_SIZE][MAX_COMMAND_SIZE];
            int i = 0;

            while (fgets(line[i], MAX_COMMAND_SIZE, ptr) != NULL)
            {
                line[i][strlen(line[i]) - 1] = '\0'; //this is the same as the history command
                                                     //that I used to read the history.txt file
                i++;
            }

            // find the length of the array
            int length;

            for (length = 0; length < MAX_COMMAND_SIZE; length++)
            {
                if (line[length][0] == '\0')
                {
                    break;
                }
            }

            int lineNumber = 0;
            if (length < 20) // if the length is less than 20 limnes, it prints everything 
            {
                for (int j = 0; j < length; j++)
                {
                    printf("%d: %s\n", lineNumber, line[j]);
                    lineNumber++;
                }
            }
            else
            {
                for (int j = length - 20; j < length; j++)
                {
                    printf("%d: %s\n", lineNumber, line[j]);
                    lineNumber++;
                }
            }
            found = 0;
            // Closing the file
            fclose(ptr);
            //
        }

        else if (token[0][0] == '!')
        {
            pid_t pid = fork(); // fork the process becuase we are going to run a command

            if (pid == 0) // if 
            {
                FILE *fp2;
                // store value of pid in int
                //  int pidValue = pid;
                //  printf("pid: %d\n", pidValue);
                int pidValue = pid;
                // write the pid to a file

                fp2 = fopen("/home/cse3320/pid.txt", "a");
                // if the value is 0, don't write it to the file
                if (pidValue != 0)
                {
                    fprintf(fp2, "%d\n", pidValue);
                }
                // we are going to open the history file and read the line number and the command
                // we will then tokenize the command and run it with execvp
                // get the number from the command
                int number = atoi(&token[0][1]);

                // open the file
                FILE *ptr;
                // Opening file in reading mode
                ptr = fopen("/home/cse3320/history.txt", "r"); 

                // read the file and get the command corresponding to the number
                char line[MAX_COMMAND_SIZE][MAX_COMMAND_SIZE];
                int i = 0;

                // read the file and store it in an array
                while (fgets(line[i], MAX_COMMAND_SIZE, ptr) != NULL)
                {
                    line[i][strlen(line[i]) - 1] = '\0';
                    i++;
                }

                // tokenize the array
                char *token2[MAX_NUM_ARGUMENTS];
                char *argument_ptr2;
                // find the length of the array
                int length;
                for (length = 0; length < MAX_COMMAND_SIZE; length++)
                {
                    if (line[length][0] == '\0')
                    {
                        break;
                    }
                }

                // if the number is greater than the length of the array
                // then print an error
                if (number > length)
                {
                    printf("Error: Command not found\n");
                    exit(0);
                }
                else
                {
                    // tokenize the command
                    char *working_str2 = strdup(line[number]);
                    // we are going to move the working_str pointer so
                    // keep track of its original value so we can deallocate
                    // the correct amount at the end
                    char *working_root2 = working_str2;
                    int token_count2 = 0;
                    while (((argument_ptr2 = strsep(&working_str2, WHITESPACE)) != NULL) &&
                           (token_count2 < MAX_NUM_ARGUMENTS))
                    {
                        token2[token_count2] = strndup(argument_ptr2, MAX_COMMAND_SIZE);
                        if (strlen(token2[token_count2]) == 0)
                        {
                            token2[token_count2] = NULL;
                        }
                        token_count2++;
                    } //this part is taken as is from the beginning of the program
                      // we are going to run the command with execvp
                      // the command is stored in token2
                    // run the command
                    execvp(token2[0], token2);
                }
                // Closing the file
                fclose(ptr);
            }
            else
            {
                wait(NULL);
            }
        }

        else
        {
            pid_t pid = fork();
            FILE *fp2;
            // store value of pid in int
            //  int pidValue = pid;
            //  printf("pid: %d\n", pidValue);
            int pidValue = pid;
            // write the pid to a file

            fp2 = fopen("/home/cse3320/pid.txt", "a"); //open the pid.txt file to write the pid
            // if the value is 0, don't write it to the file
            if (pidValue != 0)
            {
                fprintf(fp2, "%d\n", pidValue);
            }
            fclose(fp2);
            if (pid == 0)
            {
                execvp(token[0], token);
                found = 1; //set found to 1 becuase the command was not found
                           //the reason this would work is because if execvp fails
                           //it executes the code below it
            }
            else
            {
                wait(NULL);
            }
            if (pid == 0)
            {
                // convert token[0] to char*
                char *command = token[0];
                char *dir = "usr/local/bin/";
                // concatenate dir and command
                char *path; // create the char to store the path
                // we need to allocate memory for the path variable
                // we allocate certian amount of memory by adding the
                // length of the dir and command
                // add 1 to the length of the dir and command to
                // account for the null terminator
                path = (char *)(malloc(strlen(dir) + strlen(command) + 1));
                // now that we have allocated memory for the path variable we can 
                // concatenate the dir and command
                // we copy the contents of the dir first to path
                strcpy(path, dir); // this copies the dir to path
                // we then join the command to the end of the path
                strcat(path, command); // this joins the dir and command to essentially make
                                       // the correct PATH (eg:"/bin/ls") ^^
                // print the pid of the child process
                execvp(path, token);
                found = 1;
            }
            else
            {
                wait(NULL);
            }

            if (pid == 0)
            {
                // convert token[0] to char*
                char *command = token[0];
                char *dir = "usr/bin/";
                // concatenate dir and command
                char *path; // create the char to store the path
                // we need to allocate memory for the path variable
                // we allocate certian amount of memory by adding the
                // length of the dir and command
                // add 1 to the length of the dir and command to
                // account for the null terminator
                path = (char *)(malloc(strlen(dir) + strlen(command) + 1));
                // now that we have allocated memory for the path variable we can 
                //concatenate the dir and command
                // we copy the contents of the dir first to path
                strcpy(path, dir); // this copies the dir to path
                // we then join the command to the end of the path
                strcat(path, command); // this joins the dir and command to essentially make
                                       // the correct PATH (eg:"/bin/ls") ^^
                // print the pid of the child process
                execvp(path, token);
                found = 1;
            }
            else
            {
                wait(NULL);
            }
            // free the allocated memor
            // printf("Command not found   ");

            if (pid == 0)
            {
                // convert token[0] to char*
                char *command = token[0];
                char *dir = "/bin/";
                // concatenate dir and command
                char *path; // create the char to store the path
                // we need to allocate memory for the path variable
                // we allocate certian amount of memory by adding the
                // length of the dir and command
                // add 1 to the length of the dir and command to
                // account for the null terminator
                path = (char *)(malloc(strlen(dir) + strlen(command) + 1));
                // now that we have allocated memory for the path variable we can 
                // concatenate the dir and command
                // we copy the contents of the dir first to path
                strcpy(path, dir); // this copies the dir to path
                // we then join the command to the end of the path
                strcat(path, command); // this joins the dir and command to essentially make
                                       // the correct PATH (eg:"/bin/ls") ^^
                // print the pid of the child process
                execvp(path, token);
                found = 1;
            }
            else
            {
                wait(NULL);
            }
        }

        if (found == 1) //if the command is not found, it prints the error message 
        {
            printf("%s: Command not found.\n", token[0]);
            exit(0);
        }
    }
    return 0;
}