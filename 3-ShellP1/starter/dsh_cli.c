#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dshlib.h"


int main()
{
    char cmd_buff[SH_CMD_MAX]; // Buffer to store use input
    command_list_t clist;      //  hold parsed commands
    int rc;                    // code Return 

    while (1)
    {
        printf("%s", SH_PROMPT); // Display shell prompt

        // User input read
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }

        // Remove trailing newline from input
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        // Check if user entered "exit" command
        if (strcmp(cmd_buff, EXIT_CMD) == 0)
        {
            exit(OK);  // Exit 
        }

        // Parse the input and populate command list
        rc = build_cmd_list(cmd_buff, &clist);

        // Handle return codes
        if (rc == OK)
        {
            printf(CMD_OK_HEADER, clist.num);
            for (int i = 0; i < clist.num; i++)
            {
                if (strlen(clist.commands[i].args) > 0)
                {
                    printf("<%d> %s [%s]\n", i + 1, clist.commands[i].exe, clist.commands[i].args);
                }
                else
                {
                    printf("<%d> %s\n", i + 1, clist.commands[i].exe);
                }
            }
        }
        else if (rc == WARN_NO_CMDS)
        {
            printf(CMD_WARN_NO_CMD);
        }
        else if (rc == ERR_TOO_MANY_COMMANDS)
        {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
        }
    }

    return 0;
}
