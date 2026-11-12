/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_command_boundaries.c
 *
 * PURPOSE:
 *   Verify bounded owned command storage, transactional display and parsing, and copied argv ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/build/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto cleanup; } } while (0)
static int Bound(const UmiBuildCommand *command)
{
    for (size_t i = 0U; i < UMI_BUILD_MAX_ARGUMENTS; ++i)
        if (command->arguments[i] != (i < command->argument_count ? command->argument_storage[i] : NULL))
            return 0;
    return 1;
}
static void Seed(UmiBuildCommand *command)
{
    umi_build_command_init(command, "cmake");
    (void)umi_build_command_set_working_directory(command, "project with spaces");
    (void)umi_build_command_add_argument(command, "--build");
    (void)umi_build_command_add_argument(command, "binary folder");
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *scenario = argv[1];
    const char *known[] = {
        "valid",
        "empty-program",
        "unterminated-program",
        "unterminated-directory",
        "unterminated-argument",
        "excess-count",
        "maximum-count",
        "empty-argument",
        "empty-directory",
        "unused-storage",
        "stale-cache",
        "maximum-fields",
        "format-valid",
        "format-empty-argument",
        "format-quotes",
        "format-backslash",
        "format-metacharacters",
        "format-unicode",
        "format-exact",
        "format-short",
        "format-zero",
        "format-null",
        "format-count",
        "format-program",
        "format-directory",
        "format-argument",
        "copy-valid",
        "copy-self",
        "copy-stale",
        "copy-null-source",
        "copy-null-destination",
        "copy-invalid",
        "copy-independent",
        "parse-valid",
        "parse-quote",
        "parse-empty",
        "parse-argument-limit",
        "parse-long-argument",
        "parse-long-program",
        "parse-alias",
        "parse-empty-argument",
        "parse-null-text",
        "parse-null-output"
    };
    int recognised = 0;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i)
        if (!strcmp(scenario, known[i])) recognised = 1;
    if (!recognised) return 2;
    int failed = 0;
    UmiBuildCommand *command = calloc(1U, sizeof *command);
    UmiBuildCommand *saved = malloc(sizeof *saved);
    UmiBuildCommand *destination = malloc(sizeof *destination);
    char text[UMI_BUILD_MAX_ARGUMENTS * (UMI_BUILD_ARGUMENT_CAPACITY * 2U + 4U) + UMI_BUILD_PATH_CAPACITY * 2U];
    char unchanged[sizeof text], message[256];
    char longText[UMI_BUILD_PATH_CAPACITY + 1U];
    CHECK(command && saved && destination);
    Seed(command);
    memset(text, '!', sizeof text); memcpy(unchanged, text, sizeof text);
    memset(destination, 0x5a, sizeof *destination);

    if (!strncmp(scenario, "format-", 7U)) {
        const char *expected = "cmake --build \"binary folder\"";
        size_t capacity = sizeof text;
        int success = 1;
        if (!strcmp(scenario, "format-empty-argument")) {
            command->argument_count = 0U;
            CHECK(umi_build_command_add_argument(command, ""));
            expected = "cmake \"\"";
        } else if (!strcmp(scenario, "format-quotes")) {
            command->argument_count = 0U;
            CHECK(umi_build_command_add_argument(command, "a\"b"));
            expected = "cmake \"a\\\"b\"";
        } else if (!strcmp(scenario, "format-backslash")) {
            command->argument_count = 0U;
            CHECK(umi_build_command_add_argument(command, "C:\\work\\file"));
            expected = "cmake \"C:\\\\work\\\\file\"";
        } else if (!strcmp(scenario, "format-metacharacters")) {
            command->argument_count = 0U;
            CHECK(umi_build_command_add_argument(command, "$(name);a|b&c"));
            expected = "cmake $(name);a|b&c";
        } else if (!strcmp(scenario, "format-unicode")) {
            command->argument_count = 0U;
            CHECK(umi_build_command_add_argument(command, "\xe2\x82\xac"));
            expected = "cmake \xe2\x82\xac";
        } else if (!strcmp(scenario, "format-exact")) capacity = strlen(expected) + 1U;
        else if (!strcmp(scenario, "format-short")) { capacity = strlen(expected); success = 0; }
        else if (!strcmp(scenario, "format-zero")) { capacity = 0U; success = 0; }
        else if (!strcmp(scenario, "format-null")) success = 0;
        else if (!strcmp(scenario, "format-count")) { command->argument_count = SIZE_MAX; success = 0; }
        else if (!strcmp(scenario, "format-program")) { memset(command->program, 'p', sizeof command->program); success = 0; }
        else if (!strcmp(scenario, "format-directory")) { memset(command->working_directory, 'd', sizeof command->working_directory); success = 0; }
        else if (!strcmp(scenario, "format-argument")) { memset(command->argument_storage[1], 'a', sizeof command->argument_storage[1]); success = 0; }
        memcpy(saved, command, sizeof *saved);
        CHECK(umi_build_command_format(command, !strcmp(scenario, "format-null") ? NULL : text, capacity) == success);
        CHECK(!memcmp(saved, command, sizeof *saved));
        if (success) {
            CHECK(!strcmp(text, expected));
            CHECK(text[strlen(expected) + 1U] == '!');
        } else CHECK(!memcmp(text, unchanged, sizeof text));
        goto cleanup;
    }
    if (!strncmp(scenario, "copy-", 5U)) {
        if (!strcmp(scenario, "copy-stale")) {
            for (size_t i = 0U; i < UMI_BUILD_MAX_ARGUMENTS; ++i) command->arguments[i] = "foreign";
        } else if (!strcmp(scenario, "copy-invalid"))
            memset(command->working_directory, 'd', sizeof command->working_directory);
        memcpy(saved, destination, sizeof *saved);
        UmiBuildCommand *output = !strcmp(scenario, "copy-self") ? command :
            !strcmp(scenario, "copy-null-destination") ? NULL : destination;
        const UmiBuildCommand *input = !strcmp(scenario, "copy-null-source") ? NULL : command;
        UmiStatus status = umi_build_command_copy(output, input);
        if (!strcmp(scenario, "copy-invalid") || !strcmp(scenario, "copy-null-source") ||
            !strcmp(scenario, "copy-null-destination")) {
            CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(!memcmp(destination, saved, sizeof *saved));
        } else {
            CHECK(status == UMI_STATUS_OK && Bound(output));
            CHECK(!strcmp(output->arguments[1], "binary folder"));
            if (!strcmp(scenario, "copy-independent")) {
                memset(command, 0, sizeof *command);
                CHECK(!strcmp(output->arguments[0], "--build"));
                CHECK(!strcmp(output->working_directory, "project with spaces"));
            }
        }
        goto cleanup;
    }
    if (!strncmp(scenario, "parse-", 6U)) {
        const char *input = "cmake --build 'two words'";
        UmiStatus expected = UMI_STATUS_OK;
        if (!strcmp(scenario, "parse-quote")) { input = "cmake 'unfinished"; expected = UMI_STATUS_PARSE_ERROR; }
        else if (!strcmp(scenario, "parse-empty")) { input = " \t "; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(scenario, "parse-argument-limit")) {
            strcpy(text, "cmake");
            for (size_t i = 0U; i <= UMI_BUILD_MAX_ARGUMENTS; ++i) strcat(text, " x");
            input = text; expected = UMI_STATUS_CAPACITY_EXCEEDED;
        } else if (!strcmp(scenario, "parse-long-argument")) {
            strcpy(text, "cmake ");
            memset(text + 6U, 'a', UMI_BUILD_ARGUMENT_CAPACITY); text[6U + UMI_BUILD_ARGUMENT_CAPACITY] = '\0';
            input = text; expected = UMI_STATUS_CAPACITY_EXCEEDED;
        } else if (!strcmp(scenario, "parse-long-program")) {
            memset(longText, 'p', sizeof longText - 1U); longText[sizeof longText - 1U] = '\0';
            input = longText; expected = UMI_STATUS_CAPACITY_EXCEEDED;
        } else if (!strcmp(scenario, "parse-alias")) {
            strcpy(command->argument_storage[1], "tool 'owned source'");
            input = command->argument_storage[1];
        } else if (!strcmp(scenario, "parse-empty-argument")) input = "tool ''";
        else if (!strcmp(scenario, "parse-null-text")) { input = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(scenario, "parse-null-output")) expected = UMI_STATUS_INVALID_ARGUMENT;
        memcpy(saved, command, sizeof *saved);
        CHECK(umi_build_command_parse(input, !strcmp(scenario, "parse-null-output") ? NULL : command, message, sizeof message) == expected);
        if (expected != UMI_STATUS_OK) CHECK(!memcmp(command, saved, sizeof *saved));
        else {
            CHECK(Bound(command));
            if (!strcmp(scenario, "parse-alias")) CHECK(!strcmp(command->arguments[0], "owned source"));
            else if (!strcmp(scenario, "parse-empty-argument")) CHECK(command->argument_count == 1U && !strcmp(command->arguments[0], ""));
            else CHECK(command->argument_count == 2U && !strcmp(command->arguments[1], "two words"));
        }
        goto cleanup;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(scenario, "empty-program")) { command->program[0] = '\0'; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(scenario, "unterminated-program")) { memset(command->program, 'p', sizeof command->program); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(scenario, "unterminated-directory")) { memset(command->working_directory, 'd', sizeof command->working_directory); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(scenario, "unterminated-argument")) { memset(command->argument_storage[1], 'a', sizeof command->argument_storage[1]); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(scenario, "excess-count")) { command->argument_count = SIZE_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(scenario, "maximum-count")) {
        while (command->argument_count < UMI_BUILD_MAX_ARGUMENTS) CHECK(umi_build_command_add_argument(command, "x"));
    } else if (!strcmp(scenario, "empty-argument")) command->argument_storage[1][0] = '\0';
    else if (!strcmp(scenario, "empty-directory")) command->working_directory[0] = '\0';
    else if (!strcmp(scenario, "unused-storage")) memset(command->argument_storage[2], 'x', sizeof command->argument_storage[2]);
    else if (!strcmp(scenario, "stale-cache")) {
        for (size_t i = 0U; i < UMI_BUILD_MAX_ARGUMENTS; ++i) command->arguments[i] = NULL;
    } else if (!strcmp(scenario, "maximum-fields")) {
        memset(command->program, 'p', sizeof command->program - 1U); command->program[sizeof command->program - 1U] = '\0';
        memset(command->working_directory, 'd', sizeof command->working_directory - 1U); command->working_directory[sizeof command->working_directory - 1U] = '\0';
        memset(command->argument_storage[1], 'a', sizeof command->argument_storage[1] - 1U); command->argument_storage[1][sizeof command->argument_storage[1] - 1U] = '\0';
    }
    memcpy(saved, command, sizeof *saved);
    CHECK(umi_build_command_validate(command, message, sizeof message) == expected);
    CHECK(message[0] != '\0');
    CHECK(!memcmp(command, saved, sizeof *saved));
cleanup:
    free(command); free(saved); free(destination);
    return failed;
}
