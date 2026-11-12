/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_prepared_shell.c
 *
 * PURPOSE:
 *   Check prepared-shell admission and child outcomes without launching processes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/build.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus transport = UMI_STATUS_OK;
static int child_exit, child_launched = 1, child_cancelled, child_timed_out, bad_request;
static unsigned calls;
static const char *expected_program;
static UmiStatus ObserveShell(const UmiProcessRequest *request, UmiProcessResult *result)
{
    ++calls;
    if (!request || !request->program || strcmp(request->program, expected_program) ||
        !request->working_directory || strcmp(request->working_directory, "selected directory") ||
        request->window_mode != UMI_PROCESS_WINDOW_VISIBLE || request->argument_count != 0 ||
        request->environment_count != 1 || !request->environment ||
        strcmp(request->environment[0].name, "KEEP") || strcmp(request->environment[0].value, "two words"))
        bad_request = 1;
    memset(result, 0, sizeof *result);
    result->launched = child_launched;
    result->exit_code = child_exit;
    result->cancelled = child_cancelled;
    result->timed_out = child_timed_out;
    return transport;
}
/* The only process boundary is replaced. Shared environment validation and the
 * actual shell service run against owned synthetic state; no child is started. */
#define umi_process_execute ObserveShell
#include "../../src/toolchain/prepared_shell.c"
#undef umi_process_execute

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiEnvironmentPlan *owned = calloc(1U, sizeof *owned), *environment = owned;
    if (!owned) return 1;
    owned->count = 1;
    strcpy(owned->entries[0].name, "KEEP");
    strcpy(owned->entries[0].value, "two words");
    const char *program = "selected shell";
    expected_program = program;
    UmiStatus expected = UMI_STATUS_OK;
    unsigned expected_calls = 1;
    const char *mode = argv[1];
    if (!strcmp(mode, "success")) { }
    else if (!strcmp(mode, "default") || !strcmp(mode, "empty-default")) {
        program = !strcmp(mode, "default") ? NULL : "";
#ifdef _WIN32
        expected_program = "powershell.exe";
#else
        expected_program = getenv("SHELL");
        if (!expected_program || !expected_program[0]) expected_program = "/bin/sh";
#endif
    } else if (!strcmp(mode, "child-failure")) { child_exit = 19; expected = UMI_STATUS_INTERNAL_ERROR; }
    else if (!strcmp(mode, "not-launched")) { child_launched = 0; expected = UMI_STATUS_INTERNAL_ERROR; }
    else if (!strcmp(mode, "cancelled")) { child_cancelled = 1; child_exit = 19; expected = UMI_STATUS_CANCELLED; }
    else if (!strcmp(mode, "timeout")) { child_timed_out = 1; child_exit = 19; expected = UMI_STATUS_TIMEOUT; }
    else if (!strcmp(mode, "transport")) {
        transport = UMI_STATUS_IO_ERROR; child_cancelled = child_timed_out = 1;
        expected = UMI_STATUS_IO_ERROR;
    } else if (!strcmp(mode, "null-environment")) {
        environment = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; expected_calls = 0;
    } else if (!strcmp(mode, "count")) {
        owned->count = UMI_ENVIRONMENT_PLAN_MAX + 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED; expected_calls = 0;
    } else if (!strcmp(mode, "name") || !strcmp(mode, "value")) {
        if (!strcmp(mode, "name")) memset(owned->entries[0].name, 'n', sizeof owned->entries[0].name);
        else memset(owned->entries[0].value, 'v', sizeof owned->entries[0].value);
        expected = UMI_STATUS_INVALID_ARGUMENT; expected_calls = 0;
    } else if (!strcmp(mode, "duplicate")) {
        owned->count = 2; owned->entries[1] = owned->entries[0];
        expected = UMI_STATUS_ALREADY_EXISTS; expected_calls = 0;
    } else if (!strcmp(mode, "later-malformed")) {
        /* A duplicate earlier in the array must not bypass admission of a
         * later bounded field. Malformed storage takes precedence. */
        owned->count = 3; owned->entries[1] = owned->entries[0];
        memset(owned->entries[2].name, 'n', sizeof owned->entries[2].name);
        expected = UMI_STATUS_INVALID_ARGUMENT; expected_calls = 0;
    } else if (!strcmp(mode, "last-unterminated")) {
        owned->count = UMI_ENVIRONMENT_PLAN_MAX;
        for (size_t i = 1; i < owned->count; ++i) {
            snprintf(owned->entries[i].name, sizeof owned->entries[i].name, "ITEM_%zu", i);
            strcpy(owned->entries[i].value, "value");
        }
        memset(owned->entries[owned->count - 1].name, 'n', sizeof owned->entries[0].name);
        expected = UMI_STATUS_INVALID_ARGUMENT; expected_calls = 0;
    } else { free(owned); return 2; }
    UmiStatus status = umi_build_open_shell(NULL, environment, "selected directory", program);
    free(owned);
    if (status != expected || calls != expected_calls || bad_request) {
        fprintf(stderr, "%s: status=%d calls=%u request=%d; expected %d/%u\n",
            mode, (int)status, calls, bad_request, (int)expected, expected_calls);
        return 1;
    }
    return 0;
}
