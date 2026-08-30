/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_command_line.c
 *
 * PURPOSE:
 *   Verify conservative command-line parsing used by legacy project tasks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/command_line.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiDeveloperCommandLine command;

    assert(umi_developer_command_line_parse(
        "clang -std=c23 \"-DNAME=Umicom Foundation\" 'source file.c'",
        &command) == UMI_STATUS_OK);
    assert(strcmp(command.program, "clang") == 0);
    assert(command.argument_count == 3U);
    assert(strcmp(command.arguments[0], "-std=c23") == 0);
    assert(strcmp(command.arguments[1], "-DNAME=Umicom Foundation") == 0);
    assert(strcmp(command.arguments[2], "source file.c") == 0);

    assert(umi_developer_command_line_parse(
        "\"C:\\Program Files\\LLVM\\bin\\clang.exe\" -c main.c",
        &command) == UMI_STATUS_OK);
    assert(strcmp(command.program, "C:\\Program Files\\LLVM\\bin\\clang.exe") == 0);
    assert(command.argument_count == 2U);

    assert(umi_developer_command_line_parse("cmake -S . | echo bad", &command)
           == UMI_STATUS_PARSE_ERROR);
    assert(umi_developer_command_line_parse("\"unterminated", &command)
           == UMI_STATUS_PARSE_ERROR);
    return 0;
}
