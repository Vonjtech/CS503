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

/*
 * Helper function to trim leading and trailing spaces
 */
void trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) return; // Empty string

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';
}

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    
    if (cmd_line == NULL || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    // Initialize command list
    memset(clist, 0, sizeof(command_list_t));

    char *token;
    char *rest = cmd_line;
    int cmd_count = 0;

    // Split input line by pipes (`|`)
    while ((token = strtok_r(rest, PIPE_STRING, &rest))) {
        if (cmd_count >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;  // return the error
        }

        // Trim leading and trailing spaces
        trim_whitespace(token);

        // Check if command is too large
        if (strlen(token) >= SH_CMD_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG; // return the error
        }

        // Split command into executable and arguments
        char *cmd_part = strtok(token, " ");
        if (cmd_part == NULL) {
            continue;  // Skip empty commands
        }

        // Store the executable name
        strncpy(clist->commands[cmd_count].exe, cmd_part, EXE_MAX - 1);
        clist->commands[cmd_count].exe[EXE_MAX - 1] = '\0';

        // Store the arguments
        char *arg_buffer = clist->commands[cmd_count].args;
        arg_buffer[0] = '\0';

        cmd_part = strtok(NULL, " ");
        while (cmd_part) {
            strncat(arg_buffer, cmd_part, ARG_MAX - strlen(arg_buffer) - 1);
            strncat(arg_buffer, " ", ARG_MAX - strlen(arg_buffer) - 1);
            cmd_part = strtok(NULL, " ");
        }

        // Remove trailing space from arguments
        trim_whitespace(arg_buffer);

        cmd_count++
    }

    clist->num = cmd_count;
    return OK;

    
    //printf(M_NOT_IMPL);
    //return EXIT_NOT_IMPL;
}
