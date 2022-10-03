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

#define MAX_NUM_ARGUMENTS 11 // Mav shell only supports five arguments

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
        while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin)) // for !n rerun the shell with the same command line
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
        // program requirments
        // 1  ✅
        // 2  ✅ Command not found
        // 4  ✅
        // 5  ✅
        // 6  ✅
        // 7  ✅ 10 CL parameters
        // 8  ✅ Execute any command
        // 9  ✅
        // 10 ✅ cd command
        // 11 listpids command
        // 12 history and !n command

        // built in commands
        // cd, exit, listpids, history, !n (where n is a number)

        // boolean to check if the command was found
        int found = 0;

        if (token[0] == NULL)
        {
            main();
        }

        // if the user types exit, exit the shell else continue
        if (strcmp(token[0], "exit") == 0)
        {
            free(working_root);

            exit(0);
        }
        else
        {

            pid_t pid = fork();
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
                // now that we have allocated memory for the path variable we can concatenate the dir and command
                // we copy the contents of the dir first to path
                strcpy(path, dir); // this copies the dir to path
                // we then join the command to the end of the path
                strcat(path, command); // this joins the dir and command to essentially make
                                       // the correct PATH (eg:"/bin/ls") ^^
                                       // run the command
                                       // if the command is not found then the execvp will return -1
                                       // if the command is found then the execvp will return 0
                execvp(path, token);
                found = 1;
                // execvp then executes the token in the path specified.
            }
            // wait for child process to finish
            {
                wait(NULL);
            }

            if (pid == 0)
            {
                // array of pointers to char
                // convert token[0] to char*
                char *command = token[0];
                char *dir = "/usr/bin/";
                // concatenate dir and command
                char *path = malloc(strlen(dir) + strlen(command) + 1);
                strcpy(path, dir);
                strcat(path, command);
                // run the command
                found = 0;
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
                // array of pointers to char
                // convert token[0] to char*
                char *command = token[0];
                char *dir = "/bin/";
                // concatenate dir and command
                char *path = malloc(strlen(dir) + strlen(command) + 1);
                strcpy(path, dir);
                strcat(path, command);
                // run the command
                found = 0;
                execvp(path, token);
                found = 1;
            }
            else
            {
                wait(NULL);
            }
            if (strcmp(token[0], "cd") == 0)
            {
                chdir(token[1]);
                found = 0;
            }
            if (found == 1)
            {
                printf("%s: Command not found.\n", token[0]);
            }
        }

        // if (strcmp(token[0], "exit") == 0)
        // {
        //     // free the allocated memory
        //     free(working_root);
        //     printf("bruh");
        //     found = 0;
        //     printf("bruh2");
        //     // printf("Exiting the shell   ");
        //     exit(0);
        // }
    }
    return 0;
}