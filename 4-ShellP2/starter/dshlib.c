#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

 int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    memset(cmd_buff, 0, sizeof(cmd_buff_t));

    char *token;
    bool in_quotes = false;
    int argc = 0;

    cmd_buff->_cmd_buffer = strdup(cmd_line);
    if (!cmd_buff->_cmd_buffer)
        return ERR_MEMORY;

    token = strtok(cmd_buff->_cmd_buffer, " ");
    
    while (token != NULL) {
        if (argc >= CMD_ARGV_MAX - 1) {
            free(cmd_buff->_cmd_buffer);
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        if (*token == '"') {
            in_quotes = true;
            cmd_buff->argv[argc++] = token + 1;
        } else {
            cmd_buff->argv[argc++] = token;
        }

        while (in_quotes && (token = strtok(NULL, " ")) != NULL) {
            size_t len = strlen(token);
            if (token[len - 1] == '"') {
                in_quotes = false;
                token[len - 1] = '\0';
            }
            strcat(cmd_buff->argv[argc - 1], " ");
            strcat(cmd_buff->argv[argc - 1], token);
        }

        token = strtok(NULL, " ");
    }

    cmd_buff->argc = argc;
    cmd_buff->argv[argc] = NULL; 

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

        cmd_buff_t cmd;
        memset(&cmd, 0, sizeof(cmd_buff_t));
        int rc = build_cmd_buff(cmd_line, &cmd);

        if (rc != OK) {
            fprintf(stderr, "Error: Command parsing failed\n");
            continue;
        }

        if (cmd.argc > 0 && strcmp(cmd.argv[0], "cd") == 0) {
            if (cmd.argc == 1) {
            } else if (cmd.argc == 2) {
                if (chdir(cmd.argv[1]) != 0) {
                    perror("cd failed");
                }
            } else {
                fprintf(stderr, "cd: too many arguments\n");
            }
            continue;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
        } 
        else if (pid == 0) {
            execvp(cmd.argv[0], cmd.argv);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } 
        else {
            int status;
            waitpid(pid, &status, 0);
        }
    }

    return OK;
}
