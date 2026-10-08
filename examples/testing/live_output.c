/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/testing/live_output.c
 * PURPOSE: Show how a command-line host observes CTest without treating text as a verdict.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/ctest_output.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct ConsoleOutput
{
    int failed;
} ConsoleOutput;

/* The process runner owns these bytes only until this callback returns.
 * A console host may write them immediately. A GUI host must copy them into
 * bounded storage and ask its UI thread to render later, as UmiCtestJob does. */
static void print_chunk(const char *bytes, size_t length, void *context)
{
    ConsoleOutput *console = context;
    if (console->failed)
        return;
    if (fwrite(bytes, 1U, length, stdout) != length || fflush(stdout) != 0)
        console->failed = 1;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fputs("Usage: umicom-live-test-output-example <configured-build-folder> <exact-test-name>\n", stderr);
        return 2;
    }
    UmiTestResult *result = calloc(1U, sizeof(*result));
    if (result == NULL)
        return 2;
    UmiCtestRunOptions options = {0};
    options.timeout_ms = 300000U;
    ConsoleOutput console = {0};
    UmiStatus status = UmiCtestRunObserved(argv[1], argv[2], &options, print_chunk, &console, result);
    /* Console failure is separate from the test's actual result. Do not turn
     * a truncated transcript, skip, cancellation or missing report into a pass. */
    int exit_code = status == UMI_STATUS_OK && result->state == UMI_TEST_STATE_PASSED ? 0 : 1;
    if (console.failed)
    {
        fputs("\nOutput could not be written completely to the console.\n", stderr);
        exit_code = 2;
    }
    if (exit_code != 0)
        fputs(result->output, stderr);
    free(result);
    return exit_code;
}
