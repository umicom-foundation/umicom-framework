/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_terminal_command.c
 *
 * PURPOSE:
 *   Verify terminal command parsing and round-trip formatting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/terminal/command.h"

int main(void)
{
    UmiTerminalCommand command;
    char text[1024];
    umi_terminal_command_init(&command);
    assert(umi_terminal_command_parse(&command, "cmake --build build --target app") == UMI_STATUS_OK);
    assert(strcmp(umi_terminal_command_program(&command), "cmake") == 0);
    assert(command.argument_count == 5U);
    assert(umi_terminal_command_format(&command, text, sizeof(text)) == UMI_STATUS_OK);
    assert(strstr(text, "--build") != NULL);
    return 0;
}
