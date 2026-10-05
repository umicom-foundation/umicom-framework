/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/development_workflow/test_launch_arguments.c
 * PURPOSE: Verify launch argument boundaries, persistence inputs and bounded failure without running a program.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/cmake_provider.h"
#include "umicom/language_runtime/arguments.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%d: %s\n", __LINE__, #c); return 1; } } while (0)

/* Compare the vectors that Run and the DAP parser actually consume, including
 * shell-looking characters which must stay ordinary program input. */
static int Boundaries(void)
{
    UmiBuildProfile profile;
    umi_build_profile_init(&profile);
    strcpy(profile.run_program, "build/notes");
    strcpy(profile.run_arguments, "--file \"notes for review.txt\" \"\" 'caf\xc3\xa9' \"C:\\Projects\\notes\" '$HOME; echo no'");
    UmiBuildProvider provider = umi_build_cmake_provider();
    UmiBuildCommand command;
    CHECK(umi_build_provider_create_command(&provider, &profile, UMI_BUILD_PHASE_RUN, &command) == UMI_STATUS_OK);
    CHECK(command.argument_count == 6U);
    const char *expected[] = {"--file", "notes for review.txt", "", "caf\xc3\xa9", "C:\\Projects\\notes", "$HOME; echo no"};
    UmiArguments effective;
    CHECK(UmiBuildProfileArguments(&profile, &effective) == UMI_STATUS_OK);
    char encoded[4096];
    CHECK(UmiArgumentsFormat(effective.values, effective.count, encoded, sizeof(encoded)) == UMI_STATUS_OK);
    UmiLanguageRuntimeArguments debug;
    CHECK(umi_language_runtime_arguments_parse(encoded, &debug) == UMI_STATUS_OK);
    CHECK(debug.count == command.argument_count);
    for (size_t index = 0U; index < debug.count; ++index) {
        CHECK(strcmp(command.arguments[index], expected[index]) == 0);
        CHECK(strcmp(debug.values[index], command.arguments[index]) == 0);
        CHECK(debug.values[index] == debug.storage[index]);
    }
    return 0;
}

static int Literal(void)
{
    UmiBuildProfile profile, changed;
    umi_build_profile_init(&profile);
    strcpy(profile.run_argument, "--file \"notes for review.txt\" & literal");
    UmiArguments args;
    CHECK(UmiBuildProfileArguments(&profile, &args) == UMI_STATUS_OK);
    CHECK(args.count == 1U && strcmp(args.values[0], profile.run_argument) == 0);
    changed = profile;
    strcpy(changed.run_arguments, "--help");
    CHECK(!umi_build_profile_equal(&profile, &changed));
    CHECK(UmiBuildProfileArguments(&changed, &args) == UMI_STATUS_INVALID_ARGUMENT && args.count == 0U);
    changed.run_argument[0] = '\0';
    CHECK(umi_build_profile_validate(&changed, NULL, 0U) == UMI_STATUS_OK);
    CHECK(!umi_build_profile_equal(&profile, &changed));
    return 0;
}

static int Reversible(void)
{
    const char *values[] = {"", "C:\\Folder with spaces\\", "a\"b'c", "back\\slash", "line\nnext\tcolumn", "$(literal)"};
    char text[2048];
    UmiArguments parsed;
    CHECK(UmiArgumentsFormat(values, 6U, text, sizeof(text)) == UMI_STATUS_OK);
    CHECK(UmiArgumentsParse(text, &parsed) == UMI_STATUS_OK && parsed.count == 6U);
    for (size_t index = 0U; index < parsed.count; ++index)
        CHECK(strcmp(parsed.values[index], values[index]) == 0);
    strcpy(text, "previous");
    CHECK(UmiArgumentsFormat(values, 6U, text, 2U) == UMI_STATUS_CAPACITY_EXCEEDED && text[0] == '\0');
    CHECK(UmiArgumentsFormat(NULL, 0U, text, sizeof(text)) == UMI_STATUS_OK && text[0] == '\0');
    CHECK(UmiArgumentsFormat(NULL, 1U, text, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

static int Invalid(void)
{
    UmiArguments parsed;
    CHECK(UmiArgumentsParse("accepted \"unfinished", &parsed) == UMI_STATUS_PARSE_ERROR && parsed.count == 0U);
    CHECK(parsed.values[0] == NULL && parsed.storage[0][0] == '\0');
    CHECK(UmiArgumentsParse(NULL, &parsed) == UMI_STATUS_INVALID_ARGUMENT && parsed.count == 0U);
    char tooMany[128] = {0};
    for (size_t index = 0U; index < UMI_ARGUMENTS_CAPACITY + 1U; ++index) strcat(tooMany, "x ");
    CHECK(UmiArgumentsParse(tooMany, &parsed) == UMI_STATUS_CAPACITY_EXCEEDED && parsed.count == 0U);
    UmiBuildProfile profile;
    umi_build_profile_init(&profile);
    memset(profile.run_arguments, 'x', UMI_BUILD_ARGUMENT_CAPACITY);
    profile.run_arguments[UMI_BUILD_ARGUMENT_CAPACITY] = '\0';
    CHECK(UmiBuildProfileArguments(&profile, &parsed) == UMI_STATUS_CAPACITY_EXCEEDED && parsed.count == 0U);
    profile.run_arguments[UMI_BUILD_ARGUMENT_CAPACITY - 1U] = '\0';
    CHECK(UmiBuildProfileArguments(&profile, &parsed) == UMI_STATUS_OK && parsed.count == 1U);
    memset(profile.run_arguments, 'x', sizeof(profile.run_arguments));
    CHECK(umi_build_profile_validate(&profile, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "boundaries") == 0) return Boundaries();
    if (strcmp(argv[1], "literal") == 0) return Literal();
    if (strcmp(argv[1], "reversible") == 0) return Reversible();
    if (strcmp(argv[1], "invalid") == 0) return Invalid();
    return 2;
}
