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

        // Now print the tokenized input as a debug check
        // \TODO Remove this code and replace with your shell functionality

        int token_index = 0;
        for (token_index = 0; token_index < token_count; token_index++)
        {
            printf("token[%d] = %s\n", token_index, token[token_index]);
        }

        free(working_root);

        if (token[0] == NULL)
        {
            exit(0);
        }

        FILE *fp;
        fp = fopen("history.txt", "a");
        fprintf(fp, "%s", cmd_str); // this will print every line typed to a text file to be read later.
        fclose(fp);

        // if the user types exit, then exit the program
        if (strcmp(token[0], "exit") == 0)
        {
            remove("history.txt");
            exit(0); // quit the program
        }

        if (strcmp(token[0], "cd") == 0) // checks if the user has typed cd
        {
            chdir(token[1]); // changes the directory to the one specified by the user
        }

        // if the user types pwd, then print the current directory
        if (strcmp(token[0], "pwd") == 0) // checks if the user typed pwd
        {
            // chdir(token[1]); //changes the directory to the one specified by the user
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            printf("%s\n", cwd);
        }

        // if the user types ls, then list the contents of the current directory
        if (strcmp(token[0], "ls") == 0) // check if the user typed ls
        {
            // chdir(token[1]); //change directory to the one specified by the user
            pid_t pid = fork();
            if (pid == 0)
            {
                //make array of arguments that are taken from token array
                execvp(token[0], token);
            }
            else
            {
                wait(NULL);
            }
        }

        // if the user types ps, then list the current processes
        if (strcmp(token[0], "ps") == 0)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                //char *args[] = {token[0], NULL};
                execvp(token[0], token);
            }
            else
            {
                wait(NULL);
            }
        }

        // if the user types listpid, then list the last 20 processes
        if (strcmp(token[0], "listpids") == 0)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                char *args[] = {"ps", "-a", "-e", "-o", "pid", NULL};
                execvp(token[0], token);
            }
            else
            {
                wait(NULL);
            }
        }

        // if the user types cd, then change the directory
        // if (strcmp(token[0], "cd") == 0)
        // {
        //     chdir(token[1]);
        // }

        // the fucker should search for the command enterted in /bin and if it works it works
    }
    return 0;
}