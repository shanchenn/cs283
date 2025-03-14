#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

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
 int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd)
 {
     memset(cmd, 0, sizeof(cmd_buff_t));
     
     cmd->_cmd_buffer = strdup(cmd_line);
     if (!cmd->_cmd_buffer)
         return ERR_MEMORY;
     
     char *token;
     char *saveptr;
     int arg_count = 0;
     
     token = strtok_r(cmd->_cmd_buffer, " \t", &saveptr);
     
     while (token != NULL && arg_count < ARG_MAX) {
         cmd->argv[arg_count] = token;
         arg_count++;
         token = strtok_r(NULL, " \t", &saveptr);
     }
     
     cmd->argv[arg_count] = NULL;
     cmd->argc = arg_count;
     
     return OK;
 }
 
  int build_cmd_list(char *cmd_line, command_list_t *clist)
  {
      memset(clist, 0, sizeof(command_list_t));
      
      char *cmd_line_copy = strdup(cmd_line);
      if (!cmd_line_copy)
          return ERR_MEMORY;
      
      char *pipe_token;
      char *pipe_saveptr;
      int cmd_count = 0;
      
      pipe_token = strtok_r(cmd_line_copy, PIPE_STRING, &pipe_saveptr);
      
      while (pipe_token != NULL && cmd_count < CMD_MAX) {
          while (*pipe_token == SPACE_CHAR) {
              pipe_token++;
          }
          
          int rc = build_cmd_buff(pipe_token, &clist->commands[cmd_count]);
          if (rc != OK) {
              free(cmd_line_copy);
              return rc;
          }
          
          cmd_count++;
          pipe_token = strtok_r(NULL, PIPE_STRING, &pipe_saveptr);
      }
      
      free(cmd_line_copy);
      clist->num = cmd_count;
      
      if (cmd_count == 0)
          return WARN_NO_CMDS;
      
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
          
          command_list_t cmd_list;
          int rc = build_cmd_list(cmd_line, &cmd_list);
          
          if (rc != OK && rc != WARN_NO_CMDS) {
              fprintf(stderr, "Error: Command parsing failed (code %d)\n", rc);
              continue;
          }
          
          if (cmd_list.num > 0 && 
              cmd_list.commands[0].argc > 0 && 
              strcmp(cmd_list.commands[0].argv[0], "cd") == 0) {
              
              cmd_buff_t *cmd = &cmd_list.commands[0];
              if (cmd->argc == 1) {
              } else if (cmd->argc == 2) {
                  if (chdir(cmd->argv[1]) != 0) {
                      perror("cd failed");
                  }
              } else {
                  fprintf(stderr, "cd: too many arguments\n");
              }
              
              for (int i = 0; i < cmd_list.num; i++) {
                  if (cmd_list.commands[i]._cmd_buffer) {
                      free(cmd_list.commands[i]._cmd_buffer);
                  }
              }
              continue;
          }
          
          if (cmd_list.num > 1) {
              int pipes[CMD_MAX - 1][2];
              for (int i = 0; i < cmd_list.num - 1; i++) {
                  if (pipe(pipes[i]) == -1) {
                      perror("pipe creation failed");
                      
                      for (int j = 0; j < i; j++) {
                          close(pipes[j][0]);
                          close(pipes[j][1]);
                      }
                      break;
                  }
              }
              
              pid_t child_pids[CMD_MAX];
              
              for (int i = 0; i < cmd_list.num; i++) {
                  child_pids[i] = fork();
                  
                  if (child_pids[i] < 0) {
                      perror("fork failed");
                      for (int j = 0; j < cmd_list.num - 1; j++) {
                          close(pipes[j][0]);
                          close(pipes[j][1]);
                      }
                      break;
                  }
                  
                  if (child_pids[i] == 0) {
 
                      if (i > 0) {
                          if (dup2(pipes[i-1][0], STDIN_FILENO) == -1) {
                              perror("dup2 stdin failed");
                              exit(EXIT_FAILURE);
                          }
                      }
                      
                      if (i < cmd_list.num - 1) {
                          if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                              perror("dup2 stdout failed");
                              exit(EXIT_FAILURE);
                          }
                      }
                      
                      for (int j = 0; j < cmd_list.num - 1; j++) {
                          close(pipes[j][0]);
                          close(pipes[j][1]);
                      }
                      
                      execvp(cmd_list.commands[i].argv[0], cmd_list.commands[i].argv);
                      perror("execvp failed");
                      exit(EXIT_FAILURE);
                  }
              }
              
              for (int i = 0; i < cmd_list.num - 1; i++) {
                  close(pipes[i][0]);
                  close(pipes[i][1]);
              }
              
              for (int i = 0; i < cmd_list.num; i++) {
                  int status;
                  waitpid(child_pids[i], &status, 0);
              }
          } else if (cmd_list.num == 1) {
              pid_t pid = fork();
              
              if (pid < 0) {
                  perror("fork failed");
              } else if (pid == 0) {
                  execvp(cmd_list.commands[0].argv[0], cmd_list.commands[0].argv);
                  perror("execvp failed");
                  exit(EXIT_FAILURE);
              } else {
                  int status;
                  waitpid(pid, &status, 0);
              }
          }
          
          for (int i = 0; i < cmd_list.num; i++) {
              if (cmd_list.commands[i]._cmd_buffer) {
                  free(cmd_list.commands[i]._cmd_buffer);
              }
          }
      }
  
      return OK;
  }
