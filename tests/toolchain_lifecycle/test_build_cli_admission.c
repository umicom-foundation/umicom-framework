/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_build_cli_admission.c
 *
 * PURPOSE:
 *   Refuse malformed build, repair and shell commands before tool preparation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../../tools/umicom/src/cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned preparations, directory_reads;
UmiStatus umi_cli_context_prepare(UmiCliContext *context, const char *root, int gtk, int github)
{
    (void)context; (void)root; (void)gtk; (void)github;
    ++preparations;
    return UMI_STATUS_UNAVAILABLE;
}
UmiStatus umi_cli_context_prepare_operation(UmiCliContext *context,
    const char *root, UmiToolchainOperationKind operation)
{
    (void)context; (void)root; (void)operation;
    ++preparations;
    return UMI_STATUS_UNAVAILABLE;
}
static UmiStatus FixtureDirectory(char *out, size_t capacity)
{
#ifdef _WIN32
    const char *value = "C:/fixture";
#else
    const char *value = "/fixture";
#endif
    ++directory_reads;
    if (strlen(value) >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(out, value);
    return UMI_STATUS_OK;
}
/* Compile the real command boundary. Discovery always refuses, and the current
 * directory is synthetic, so no case builds, repairs, writes or opens a shell. */
#define umi_fs_current_directory FixtureDirectory
#include "../../tools/umicom/src/command_build.c"
#undef umi_fs_current_directory

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = strchr(argv[1], '.');
    if (!mode) return 2;
    ++mode;
    int command = !strncmp(argv[1], "build.", 6) ? 0 :
        !strncmp(argv[1], "repair.", 7) ? 1 :
        !strncmp(argv[1], "shell.", 6) ? 2 : -1;
    if (command < 0) return 2;
    UmiCliContext *owned = calloc(1U, sizeof *owned), *context = owned;
    if (!owned) return 1;
    char *values[3] = {"--help", NULL, NULL};
    char **arguments = values;
    int count = 1, expected = 2;
    unsigned expected_preparations = 0, expected_directories = 0;
    if (!strcmp(mode, "null-vector")) arguments = NULL;
    else if (!strcmp(mode, "null-entry")) values[0] = NULL;
    else if (!strcmp(mode, "negative")) count = -1;
    else if (!strcmp(mode, "null-context")) { context = NULL; count = 0; }
    else if (!strcmp(mode, "help")) expected = 0;
    else if (!strcmp(mode, "help-short")) { values[0] = "help"; expected = 0; }
    else if (!strcmp(mode, "empty")) { values[0] = ""; expected_directories = command == 0; }
    else if (!strcmp(mode, "unknown")) { values[0] = "--unknown"; expected_directories = command == 0; }
    else if (!strcmp(mode, "no-options")) {
        count = 0; arguments = NULL; expected = 1; expected_preparations = 1;
        expected_directories = command == 0;
    } else if (!strcmp(mode, "dry-run") && command == 1) {
        values[0] = "--dry-run"; expected = 1; expected_preparations = 1;
    } else if (!strcmp(mode, "duplicate") && command == 1) {
        values[0] = values[1] = "--dry-run"; count = 2;
    } else if (!strcmp(mode, "extra") && command == 1) {
        values[0] = "--dry-run"; values[1] = "--unknown"; count = 2;
    } else if (!strcmp(mode, "program") && command == 2) {
        values[0] = "path with spaces/shell"; expected = 1; expected_preparations = 1;
    } else if (!strcmp(mode, "extra") && command == 2) {
        values[0] = "shell"; values[1] = "ignored-command"; count = 2;
    } else { free(owned); return 2; }
    int result = command == 0 ? umi_cli_command_build(context, UMI_BUILD_COMPILE, count, arguments) :
        command == 1 ? umi_cli_command_repair(context, count, arguments) :
        umi_cli_command_shell(context, count, arguments);
    free(owned);
    if (result != expected || preparations != expected_preparations ||
        directory_reads != expected_directories) {
        fprintf(stderr, "%s: result=%d preparation=%u directory=%u; expected %d/%u/%u\n",
            argv[1], result, preparations, directory_reads,
            expected, expected_preparations, expected_directories);
        return 1;
    }
    return 0;
}
