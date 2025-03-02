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

 // Allocates memory for cmd_buff
int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
    cmd_buff->_cmd_buffer = (char *)malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer)
        return ERR_MEMORY;
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    return OK;
}

// Frees allocated memory for cmd_buff
int free_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (cmd_buff->_cmd_buffer)
    {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    return OK;
}

// Clears the command buffer before parsing new input
int clear_cmd_buff(cmd_buff_t *cmd_buff)
{
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++)
    {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}

// Function to parse the input command string into a structured cmd_buff_t
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    if (!cmd_line || !cmd_buff)
        return WARN_NO_CMDS;

    // Clear any previous data in the command buffer.
    clear_cmd_buff(cmd_buff);

    int argc = 0;
    int i = 0;

    // Loop over the input command line.
    while (cmd_line[i] != '\0')
    {
        // Skip any leading whitespace.
        while (cmd_line[i] != '\0' && isspace(cmd_line[i]))
            i++;

        if (cmd_line[i] == '\0')
            break; // reached the end

        if (argc >= CMD_ARGV_MAX - 1)
            break; // reached the maximum allowed arguments

        char *token = NULL;

        // If the token starts with a double quote, handle it specially.
        if (cmd_line[i] == '\"')
        {
            i++; // Skip the opening quote.
            int start = i;
            // Continue until the closing quote or end-of-string.
            while (cmd_line[i] != '\0' && cmd_line[i] != '\"')
                i++;
            int len = i - start;
            token = malloc(len + 1);
            if (!token)
                return ERR_MEMORY;
            strncpy(token, &cmd_line[start], len);
            token[len] = '\0';
            // If we found a closing quote, skip it.
            if (cmd_line[i] == '\"')
                i++;
        }
        else
        {
            // For unquoted tokens, read until the next whitespace.
            int start = i;
            while (cmd_line[i] != '\0' && !isspace(cmd_line[i]))
                i++;
            int len = i - start;
            token = malloc(len + 1);
            if (!token)
                return ERR_MEMORY;
            strncpy(token, &cmd_line[start], len);
            token[len] = '\0';
        }
        // Save the token in the argument array.
        cmd_buff->argv[argc++] = token;
    }

    // Null-terminate the argv list and update the argument count.
    cmd_buff->argv[argc] = NULL;
    cmd_buff->argc = argc;

    return (argc > 0) ? OK : WARN_NO_CMDS;
}

 int exec_local_cmd_loop()
 {
     char input_buff[SH_CMD_MAX];
     int rc = 0;
     cmd_buff_t cmd;
     int cmd_count = 0;  // Count of commands processed
 
     // In interactive mode, we assume the driver (dsh_cli.c) handles prompt printing.
     bool interactive = isatty(STDIN_FILENO);
 
     if (alloc_cmd_buff(&cmd) != OK) {
         fprintf(stderr, "Error allocating memory for command buffer\n");
         exit(ERR_MEMORY);
     }
 
     while (1)
     {
         if (interactive) {
             printf("%s", SH_PROMPT);
             fflush(stdout);
         }
 
         if (fgets(input_buff, SH_CMD_MAX, stdin) == NULL) {
             printf("\n");
             break;
         }
         // Remove trailing newline.
         input_buff[strcspn(input_buff, "\n")] = '\0';
 
         // If the input is empty, continue.
         if (strlen(input_buff) == 0)
             continue;
 
         /* --- PARSE PIPE-SEPARATED COMMANDS --- */
         // For simplicity, assume a maximum number of piped commands (e.g., 10).
         #define MAX_PIPE_CMDS 10
         char *pipe_cmds[MAX_PIPE_CMDS];
         int num_pipe_cmds = 0;
 
         // Use strtok to split by the pipe character.
         char *token = strtok(input_buff, "|");
         while (token != NULL && num_pipe_cmds < MAX_PIPE_CMDS) {
             // Remove any leading/trailing whitespace from each token.
             while (isspace(*token)) token++;
             char *end = token + strlen(token) - 1;
             while (end > token && isspace(*end)) {
                 *end = '\0';
                 end--;
             }
             pipe_cmds[num_pipe_cmds++] = token;
             token = strtok(NULL, "|");
         }
 
         if (num_pipe_cmds == 0)
             continue;
 
         // Check for too many piped commands.
         if (num_pipe_cmds >= MAX_PIPE_CMDS) {
             fprintf(stderr, CMD_ERR_PIPE_LIMIT, MAX_PIPE_CMDS);
             continue;
         }
 
         /* --- HANDLE SINGLE COMMAND (no pipes) --- */
         if (num_pipe_cmds == 1) {
             clear_cmd_buff(&cmd);
             rc = build_cmd_buff(pipe_cmds[0], &cmd);
             if (rc == WARN_NO_CMDS)
                 continue;
             if (strcmp(cmd.argv[0], EXIT_CMD) == 0)
                 break;
             if (strcmp(cmd.argv[0], "cd") == 0) {
                 if (cmd.argc > 1) {
                     if (chdir(cmd.argv[1]) != 0)
                         perror("cd");
                 }
             } else {
                 pid_t pid = fork();
                 if (pid == -1) {
                     perror("fork");
                 } else if (pid == 0) {
                     execvp(cmd.argv[0], cmd.argv);
                     perror(CMD_ERR_EXECUTE);
                     exit(ERR_EXEC_CMD);
                 } else {
                     int status;
                     waitpid(pid, &status, 0);
                 }
             }
         }
         /* --- HANDLE PIPED COMMANDS --- */
         else {
             int num_cmds = num_pipe_cmds;
             pid_t pids[num_cmds];
             int prev_pipe_fd[2];      // file descriptors for the previous pipe
             bool has_prev_pipe = false;
 
             for (int i = 0; i < num_cmds; i++) {
                 int curr_pipe_fd[2];
 
                 // For all but the last command, create a pipe.
                 if (i < num_cmds - 1) {
                     if (pipe(curr_pipe_fd) < 0) {
                         perror("pipe");
                         exit(ERR_MEMORY);
                     }
                 }
 
                 clear_cmd_buff(&cmd);
                 rc = build_cmd_buff(pipe_cmds[i], &cmd);
                 if (rc == WARN_NO_CMDS)
                     continue;
 
                 // Fork a child for this command.
                 pids[i] = fork();
                 if (pids[i] < 0) {
                     perror("fork");
                     exit(ERR_MEMORY);
                 }
 
                 if (pids[i] == 0) {  // Child process
                     // If not the first command, set up STDIN from previous pipe.
                     if (has_prev_pipe) {
                         if (dup2(prev_pipe_fd[0], STDIN_FILENO) < 0) {
                             perror("dup2");
                             exit(ERR_EXEC_CMD);
                         }
                     }
                     // If not the last command, set up STDOUT to current pipe.
                     if (i < num_cmds - 1) {
                         if (dup2(curr_pipe_fd[1], STDOUT_FILENO) < 0) {
                             perror("dup2");
                             exit(ERR_EXEC_CMD);
                         }
                     }
                     // Close unused descriptors in child.
                     if (has_prev_pipe) {
                         close(prev_pipe_fd[0]);
                         close(prev_pipe_fd[1]);
                     }
                     if (i < num_cmds - 1) {
                         close(curr_pipe_fd[0]);
                         close(curr_pipe_fd[1]);
                     }
                     // Execute the command.
                     execvp(cmd.argv[0], cmd.argv);
                     perror(CMD_ERR_EXECUTE);
                     exit(ERR_EXEC_CMD);
                 }
                 // Parent process:
                 // Close previous pipe (if any) as they are no longer needed.
                 if (has_prev_pipe) {
                     close(prev_pipe_fd[0]);
                     close(prev_pipe_fd[1]);
                 }
                 // If not the last command, set current pipe as the previous pipe for the next iteration.
                 if (i < num_cmds - 1) {
                     prev_pipe_fd[0] = curr_pipe_fd[0];
                     prev_pipe_fd[1] = curr_pipe_fd[1];
                     has_prev_pipe = true;
                 }
             }
 
             // In case the last created pipe wasn’t closed.
             if (has_prev_pipe) {
                 close(prev_pipe_fd[0]);
                 close(prev_pipe_fd[1]);
             }
             // Wait for all child processes to finish.
             for (int i = 0; i < num_cmds; i++) {
                 int status;
                 waitpid(pids[i], &status, 0);
             }
         }
         cmd_count++;  // Count this iteration.
     }
 
     if (!interactive) {
         for (int i = 0; i < cmd_count + 1; i++) {
             printf("%s", SH_PROMPT);
         }
         fflush(stdout);
     }
 
     free_cmd_buff(&cmd);
     return OK;
 }