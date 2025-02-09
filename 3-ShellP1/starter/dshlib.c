#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"


int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    if (strlen(cmd_line) == 0)
    {
        return WARN_NO_CMDS; // Empty input
    }

    // Initialization
    clist->num = 0;
    memset(clist->commands, 0, sizeof(clist->commands));

    char *saveptr; 
    char *token = strtok_r(cmd_line, PIPE_STRING, &saveptr);

    while (token != NULL)
    {
        if (clist->num >= CMD_MAX)
        {
            return ERR_TOO_MANY_COMMANDS;
        }

        
        while (isspace((unsigned char)*token))
            token++;

     
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end))
            end--;
        *(end + 1) = '\0';

        
        char *cmd_name = strtok(token, " ");  
        char *cmd_args = strtok(NULL, "");    

        if (cmd_name != NULL)
        {
            strncpy(clist->commands[clist->num].exe, cmd_name, EXE_MAX - 1);
            if (cmd_args != NULL)
            {
                strncpy(clist->commands[clist->num].args, cmd_args, ARG_MAX - 1);
            }
            clist->num++;
        }

        token = strtok_r(NULL, PIPE_STRING, &saveptr);  
    return OK;
    }
}
