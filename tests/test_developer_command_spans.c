/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_command_spans.c
 *
 * PURPOSE:
 *   Verify exact-span command parsing, empty arguments and failure atomicity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "support/assertions.h"
#include "umicom/developer/command_line.h"

#include <assert.h>
#include <string.h>

/* Rejection must leave a previously usable plan untouched, including metadata. */
static void unchanged_failure(const char *text, size_t size, UmiStatus expected)
{
    UmiDeveloperCommandLine command;
    unsigned char before[sizeof(command)];
    memset(&command, 0x5A, sizeof(command));
    memcpy(before, &command, sizeof(command));
    assert(umi_developer_command_line_parse_n(text, size, &command) == expected);
    assert(memcmp(before, &command, sizeof(command)) == 0);
}

static void empty_and_joined_arguments(void)
{
    UmiDeveloperCommandLine command;
    assert(umi_developer_command_line_parse(
        "tool \"\" '' before\"\"after a\" b \"c", &command) == UMI_STATUS_OK);
    assert(strcmp(command.program, "tool") == 0);
    assert(command.argument_count == 4U);
    assert(command.arguments[0][0] == '\0');
    assert(command.arguments[1][0] == '\0');
    assert(strcmp(command.arguments[2], "beforeafter") == 0);
    assert(strcmp(command.arguments[3], "a b c") == 0);
    unchanged_failure("\"\" tool", 7U, UMI_STATUS_PARSE_ERROR);
}

static void exact_span_and_embedded_zero(void)
{
    const char text[] = {'t', 'o', 'o', 'l', ' ', '"', '"', '|'};
    const char hidden[] = {'t', 'o', 'o', 'l', '\0', 'x'};
    UmiDeveloperCommandLine command;
    assert(umi_developer_command_line_parse_n(text, 7U, &command) == UMI_STATUS_OK);
    assert(command.argument_count == 1U);
    assert(command.arguments[0][0] == '\0');
    unchanged_failure(text, sizeof(text), UMI_STATUS_PARSE_ERROR);
    unchanged_failure(hidden, sizeof(hidden), UMI_STATUS_PARSE_ERROR);
    unchanged_failure("", 0U, UMI_STATUS_PARSE_ERROR);
    unchanged_failure(" \t ", 3U, UMI_STATUS_PARSE_ERROR);
}

static void malformed_input_preserves_previous_plan(void)
{
    const char *bad[] = {"tool \"unfinished", "tool 'unfinished", "tool | next",
        "tool & next", "tool ; next", "tool < file", "tool > file",
        "tool (arg)", "tool `arg`", "tool\nnext", "tool\rnext"};
    UmiDeveloperCommandLine command;
    unsigned char before[sizeof(command)];
    size_t index;
    assert(umi_developer_command_line_parse("compiler -c main.c", &command)
        == UMI_STATUS_OK);
    memcpy(before, &command, sizeof(command));
    for (index = 0U; index < sizeof(bad) / sizeof(bad[0]); ++index) {
        assert(umi_developer_command_line_parse(bad[index], &command)
            == UMI_STATUS_PARSE_ERROR);
        assert(memcmp(before, &command, sizeof(command)) == 0);
    }
    assert(umi_developer_command_line_parse_n(NULL, 1U, &command)
        == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(before, &command, sizeof(command)) == 0);
    assert(umi_developer_command_line_parse_n("tool", 4U, NULL)
        == UMI_STATUS_INVALID_ARGUMENT);
}

static void quoted_data_and_legacy_escapes(void)
{
    UmiDeveloperCommandLine command;
    assert(umi_developer_command_line_parse(
        "\"C:\\Program Files\\LLVM\\bin\\clang.exe\" \"a|b;c\" "
        "source\\ file.c C:\\src\\main.c", &command) == UMI_STATUS_OK);
    assert(strcmp(command.program, "C:\\Program Files\\LLVM\\bin\\clang.exe") == 0);
    assert(command.argument_count == 3U);
    assert(strcmp(command.arguments[0], "a|b;c") == 0);
    assert(strcmp(command.arguments[1], "source file.c") == 0);
    assert(strcmp(command.arguments[2], "C:\\src\\main.c") == 0);
    assert(umi_developer_command_line_parse("tool tail\\", &command) == UMI_STATUS_OK);
    assert(strcmp(command.arguments[0], "tail\\") == 0);
    assert(umi_developer_command_line_parse("tool \\\"quoted\\\"", &command)
        == UMI_STATUS_OK);
    assert(strcmp(command.arguments[0], "\"quoted\"") == 0);
}

static void argument_count_boundary(void)
{
    char text[4U + (UMI_DEVELOPER_MAX_ARGUMENTS + 1U) * 3U + 1U] = "tool";
    UmiDeveloperCommandLine command;
    size_t index, length = 4U;
    for (index = 0U; index < UMI_DEVELOPER_MAX_ARGUMENTS; ++index) {
        memcpy(text + length, " \"\"", 3U);
        length += 3U;
    }
    text[length] = '\0';
    assert(umi_developer_command_line_parse_n(text, length, &command)
        == UMI_STATUS_OK);
    assert(command.argument_count == UMI_DEVELOPER_MAX_ARGUMENTS);
    for (index = 0U; index < command.argument_count; ++index)
        assert(command.arguments[index][0] == '\0');
    memcpy(text + length, " \"\"", 3U);
    unchanged_failure(text, length + 3U, UMI_STATUS_CAPACITY_EXCEEDED);
}

static void token_capacity_boundaries(void)
{
    char program[UMI_DEVELOPER_PATH_CAPACITY];
    char argument[5U + UMI_DEVELOPER_ARGUMENT_CAPACITY];
    UmiDeveloperCommandLine command;
    memset(program, 'p', sizeof(program));
    assert(umi_developer_command_line_parse_n(
        program, sizeof(program) - 1U, &command) == UMI_STATUS_OK);
    assert(strlen(command.program) == sizeof(program) - 1U);
    unchanged_failure(program, sizeof(program), UMI_STATUS_CAPACITY_EXCEEDED);
    memcpy(argument, "tool ", 5U);
    memset(argument + 5U, 'a', sizeof(argument) - 5U);
    assert(umi_developer_command_line_parse_n(
        argument, sizeof(argument) - 1U, &command) == UMI_STATUS_OK);
    assert(strlen(command.arguments[0]) == UMI_DEVELOPER_ARGUMENT_CAPACITY - 1U);
    unchanged_failure(argument, sizeof(argument), UMI_STATUS_CAPACITY_EXCEEDED);
}

static void overlapping_input_and_output(void)
{
    UmiDeveloperCommandLine command = {0};
    strcpy(command.program, "tool \"\" tail");
    assert(umi_developer_command_line_parse(command.program, &command)
        == UMI_STATUS_OK);
    assert(strcmp(command.program, "tool") == 0);
    assert(command.argument_count == 2U);
    assert(command.arguments[0][0] == '\0');
    assert(strcmp(command.arguments[1], "tail") == 0);
}

int main(void)
{
    empty_and_joined_arguments();
    exact_span_and_embedded_zero();
    malformed_input_preserves_previous_plan();
    quoted_data_and_legacy_escapes();
    argument_count_boundary();
    token_capacity_boundaries();
    overlapping_input_and_output();
    return 0;
}
