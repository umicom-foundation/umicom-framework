/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/setup_centre/test_native_cli.c
 *
 * PURPOSE:
 *   Test native setup invocation ownership using a recording executor boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/setup_centre/execution.h"
#include <stdio.h>
#include <string.h>

static int expectedArgc, calls, contractFailed;
static char **expectedArgv;
static uint32_t expectedTimeout;

/* These recording ports test the native entry's ownership only. Real command
 * parsing and processes are covered by command equivalence and process tests. */
UmiStatus UmiSetupExecuteProcess(const char *program, const char *const *arguments,
    size_t count, const char *directory, void *context, int *exitCode)
{
    (void)program; (void)arguments; (void)count; (void)directory;
    (void)context; (void)exitCode;
    contractFailed = 1;
    return UMI_STATUS_INVALID_STATE;
}

int UmiSetupMainWithExecutor(int argc, char **argv, UmiSetupExecute execute, void *context)
{
    ++calls;
    UmiSetupProcessContext *native = context;
    if (argc != expectedArgc || argv != expectedArgv || execute != UmiSetupExecuteProcess ||
        native == NULL || native->result == NULL || native->cancellation != NULL ||
        native->timeoutMs != expectedTimeout || native->result->output[0] != '\0') {
        contractFailed = 1;
        return 1;
    }
    /* UTF-8 bytes and option order are borrowed, not rewritten by this layer. */
    for (int i = 0; i < argc; ++i)
        if (argv[i] != expectedArgv[i]) contractFailed = 1;
    (void)snprintf(native->result->output, sizeof native->result->output,
        "Recording setup command completed without launching a child.\n");
    return 19;
}

static int CheckCall(int argc, char **argv, uint32_t timeout)
{
    expectedArgc = argc; expectedArgv = argv; expectedTimeout = timeout;
    int before = calls;
    return UmiSetupNativeMain(argc, argv) == 19 && calls == before + 1 && !contractFailed;
}

int main(void)
{
    char *help[] = {"setup", "--help", NULL};
    char *interactive[] = {"setup", "vm-run", "--interactive", "yes", NULL};
    char *bounded[] = {"setup", "vm-run", "--interactive", "no", NULL};
    char *other[] = {"setup", "media-iso", "--interactive", "yes", NULL};
    char *unicode[] = {"setup", "media-stage", "--kernel", "C:/Practice/caf\xc3\xa9/kernel", NULL};
    char *missing[] = {"setup", NULL};
    if (!CheckCall(2, help, 300000U) || !CheckCall(4, interactive, 0U) ||
        !CheckCall(4, bounded, 300000U) || !CheckCall(4, other, 300000U) ||
        !CheckCall(4, unicode, 300000U)) return 1;
    int before = calls;
    if (UmiSetupNativeMain(0, help) != 2 || UmiSetupNativeMain(1, NULL) != 2 ||
        UmiSetupNativeMain(2, missing) != 2 || calls != before) return 1;
    return contractFailed;
}
