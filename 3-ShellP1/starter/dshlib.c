#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
 int build_cmd_list(char *cmd_line, command_list_t *cmd_list)
 {
     memset(cmd_list, 0, sizeof(command_list_t));
     
     // Create a copy of the input for tokenization
     char *cmd_line_copy = strdup(cmd_line);
     if (!cmd_line_copy)
         return ERR_MEMORY;
     
     char *pipe_token;
     char *pipe_saveptr;
     int cmd_count = 0;
     
     // Split by pipe symbol
     pipe_token = strtok_r(cmd_line_copy, "|", &pipe_saveptr);
     
     while (pipe_token != NULL && cmd_count < CMD_PIPE_MAX) {
         // Trim leading whitespace
         while (*pipe_token == ' ') pipe_token++;
         
         // Create a copy of this command string for parsing
         char *cmd_str = strdup(pipe_token);
         if (!cmd_str) {
             free(cmd_line_copy);
             return ERR_MEMORY;
         }
         
         // Parse the command and its arguments
         cmd_buff_t *cmd = &cmd_list->commands[cmd_count];
         memset(cmd, 0, sizeof(cmd_buff_t));
         
         cmd->_cmd_buffer = cmd_str;
         
         // Tokenize by spaces to get arguments
         char *arg_token;
         bool in_quotes = false;
         int argc = 0;
         
         arg_token = strtok(cmd_str, " ");
         
         while (arg_token != NULL) {
             if (argc >= CMD_ARGV_MAX - 1) {
                 free(cmd_line_copy);
                 return ERR_CMD_OR_ARGS_TOO_BIG;
             }
             
             if (*arg_token == '"') {
                 in_quotes = true;
                 cmd->argv[argc++] = arg_token + 1;
             } else {
                 cmd->argv[argc++] = arg_token;
             }
             
             while (in_quotes && (arg_token = strtok(NULL, " ")) != NULL) {
                 size_t len = strlen(arg_token);
                 if (arg_token[len - 1] == '"') {
                     in_quotes = false;
                     arg_token[len - 1] = '\0';
                 }
                 strcat(cmd->argv[argc - 1], " ");
                 strcat(cmd->argv[argc - 1], arg_token);
             }
             
             arg_token = strtok(NULL, " ");
         }
         
         cmd->argc = argc;
         cmd->argv[argc] = NULL;
         
         cmd_count++;
         pipe_token = strtok_r(NULL, "|", &pipe_saveptr);
     }
     
     free(cmd_line_copy);
     cmd_list->cmd_count = cmd_count;
     
     if (cmd_count == 0)
         return ERR_NO_COMMANDS;
     
     return OK;
 }
 
 int exec_local_cmd_loop()
 {
     char cmd_line[SH_CMD_MAX];
 
     while (1) {
         printf("%s", SH_PROMPT);
 
         if (fgets(cmd_line, SH_CMD_MAX, stdin) == NULL) {
             printf("\n");
             break;
         }
 
         cmd_line[strcspn(cmd_line, "\n")] = '\0';
 
         if (strcmp(cmd_line, EXIT_CMD) == 0) {
             exit(0);
         }
 
         if (strlen(cmd_line) == 0) {
             printf(CMD_WARN_NO_CMD);
             continue;
         }
         
         // Parse the command line into a list of piped commands
         command_list_t cmd_list;
         int rc = build_cmd_list(cmd_line, &cmd_list);
         
         if (rc != OK) {
             fprintf(stderr, "Error: Command parsing failed (code %d)\n", rc);
             continue;
         }
         
         // Handle built-in commands (for first command only)
         if (cmd_list.cmd_count > 0 && 
             cmd_list.commands[0].argc > 0 && 
             strcmp(cmd_list.commands[0].argv[0], "cd") == 0) {
             
             cmd_buff_t *cmd = &cmd_list.commands[0];
             if (cmd->argc == 1) {
                 // cd to home directory would go here
             } else if (cmd->argc == 2) {
                 if (chdir(cmd->argv[1]) != 0) {
                     perror("cd failed");
                 }
             } else {
                 fprintf(stderr, "cd: too many arguments\n");
             }
             
             // Clean up resources
             for (int i = 0; i < cmd_list.cmd_count; i++) {
                 if (cmd_list.commands[i]._cmd_buffer) {
                     free(cmd_list.commands[i]._cmd_buffer);
                 }
             }
             continue;
         }
         
         // Execute pipeline - all piping logic is incorporated in this part
         
         // Create pipes for all commands except the last one
         int pipes[CMD_PIPE_MAX - 1][2];
         for (int i = 0; i < cmd_list.cmd_count - 1; i++) {
             if (pipe(pipes[i]) == -1) {
                 perror("pipe creation failed");
                 
                 // Clean up resources
                 for (int j = 0; j < i; j++) {
                     close(pipes[j][0]);
                     close(pipes[j][1]);
                 }
                 break;
             }
         }
         
         // Array to store child PIDs
         pid_t child_pids[CMD_PIPE_MAX];
         
         // Fork and execute each command
         for (int i = 0; i < cmd_list.cmd_count; i++) {
             child_pids[i] = fork();
             
             if (child_pids[i] < 0) {
                 perror("fork failed");
                 break;
             }
             
             if (child_pids[i] == 0) { // Child process
                 // Set up pipe redirection
                 
                 // If not the first command, set stdin to read from previous pipe
                 if (i > 0) {
                     if (dup2(pipes[i-1][0], STDIN_FILENO) == -1) {
                         perror("dup2 stdin failed");
                         exit(EXIT_FAILURE);
                     }
                 }
                 
                 // If not the last command, set stdout to write to next pipe
                 if (i < cmd_list.cmd_count - 1) {
                     if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                         perror("dup2 stdout failed");
                         exit(EXIT_FAILURE);
                     }
                 }
                 
                 // Close all pipe file descriptors in the child
                 for (int j = 0; j < cmd_list.cmd_count - 1; j++) {
                     close(pipes[j][0]);
                     close(pipes[j][1]);
                 }
                 
                 // Execute the command
                 execvp(cmd_list.commands[i].argv[0], cmd_list.commands[i].argv);
                 perror("execvp failed");
                 exit(EXIT_FAILURE);
             }
         }
         
         // Parent process: close all pipe file descriptors
         for (int i = 0; i < cmd_list.cmd_count - 1; i++) {
             close(pipes[i][0]);
             close(pipes[i][1]);
         }
         
         // Wait for all child processes to finish
         for (int i = 0; i < cmd_list.cmd_count; i++) {
             int status;
             waitpid(child_pids[i], &status, 0);
         }
         
         // Clean up resources
         for (int i = 0; i < cmd_list.cmd_count; i++) {
             if (cmd_list.commands[i]._cmd_buffer) {
                 free(cmd_list.commands[i]._cmd_buffer);
             }
         }
     }
 
     return OK;
 }