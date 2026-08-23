/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_command_parser.c
 *
 * PURPOSE:
 *   Verify shell-independent command parsing preserves argv exactly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/build/types.h"

int main(void)
{
    UmiBuildCommand command;
    char message[256];
    char display[4096];

    assert(umi_build_command_parse(
        "tool --name \"alpha beta\" '' '$HOME' 'a|b' escaped\\ value",
        &command,
        message,
        sizeof(message)) == UMI_STATUS_OK);

    assert(strcmp(command.program, "tool") == 0);
    assert(command.argument_count == 6U);
    assert(strcmp(command.arguments[0], "--name") == 0);
    assert(strcmp(command.arguments[1], "alpha beta") == 0);
    assert(strcmp(command.arguments[2], "") == 0);
    assert(strcmp(command.arguments[3], "$HOME") == 0);
    assert(strcmp(command.arguments[4], "a|b") == 0);
    assert(strcmp(command.arguments[5], "escaped value") == 0);

    assert(umi_build_command_format(
        &command, display, sizeof(display)) != 0);
    assert(umi_build_command_parse(
        "\"C:\\Program Files\\Umicom\\tool.exe\" C:\\work\\src",
        &command,
        message,
        sizeof(message)) == UMI_STATUS_OK);
    assert(strcmp(command.program,
                  "C:\\Program Files\\Umicom\\tool.exe") == 0);
    assert(strcmp(command.arguments[0], "C:\\work\\src") == 0);
    assert(strstr(display, "alpha beta") != NULL);

    assert(umi_build_command_parse(
        "tool \"unterminated",
        &command,
        message,
        sizeof(message)) == UMI_STATUS_PARSE_ERROR);

    assert(umi_build_command_parse(
        "",
        &command,
        message,
        sizeof(message)) == UMI_STATUS_INVALID_ARGUMENT);

    return 0;
}
