/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_terminal_command.c
 *
 * PURPOSE:
 *   Verify terminal command parsing and round-trip formatting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/terminal/command.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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
