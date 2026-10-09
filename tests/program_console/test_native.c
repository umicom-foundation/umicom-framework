/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/program_console/test_native.c
 * PURPOSE: Exercise real console input, cancellation, child environment and bounded output.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    const char *mode = argv[1], *producer = "echo";
    const char *known[] = {"echo",  "unicode", "binary",      "eof",  "stop", "timeout",
                           "large", "exit",    "environment", "copy", "busy"};
    bool valid = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i)
        if (strcmp(mode, known[i]) == 0)
            valid = true;
    if (!valid)
        return 2;
    if (strcmp(mode, "stop") == 0 || strcmp(mode, "timeout") == 0 || strcmp(mode, "busy") == 0)
        producer = "wait";
    if (strcmp(mode, "large") == 0)
        producer = "large";
    if (strcmp(mode, "exit") == 0)
        producer = "exit";
    if (strcmp(mode, "environment") == 0)
        producer = "environment";
    if (strcmp(mode, "binary") == 0)
        producer = "nul";
    int failed = 0;
    UmiProgramConsole *console = NULL;
    UmiThread *thread = NULL;
    UmiProgramConsoleSnapshot *snapshot = calloc(1U, sizeof *snapshot);
    CHECK(snapshot != NULL);
    CHECK(ConsoleTestCreate(argv[2], producer, strcmp(mode, "timeout") == 0 ? 100U : 5000U,
                            strcmp(mode, "environment") == 0
                                ? "UMICOM_CONSOLE_FIXTURE_VALUE='caf\xc3\xa9 value'"
                                : NULL,
                            &console) == UMI_STATUS_OK);
    char message[64] = "owned input\n";
    if (strcmp(mode, "unicode") == 0)
        strcpy(message, "caf\xc3\xa9 \xe2\x82\xac\n");
    size_t message_length = strlen(message);
    if (strcmp(producer, "echo") == 0 || strcmp(producer, "nul") == 0)
    {
        if (strcmp(mode, "eof") != 0)
            CHECK(UmiProgramConsoleSend(console, message, message_length) == UMI_STATUS_OK);
        if (strcmp(mode, "copy") == 0)
            memset(message, '?', message_length);
        CHECK(UmiProgramConsoleEndInput(console) == UMI_STATUS_OK);
    }
    CHECK(umi_thread_start(ConsoleTestWorker, console, &thread) == UMI_STATUS_OK);
    if (strcmp(mode, "stop") == 0 || strcmp(mode, "busy") == 0)
    {
        CHECK(ConsoleTestWait(console, snapshot, 1));
        CHECK(snapshot->started && !snapshot->completed);
        if (strcmp(mode, "busy") == 0)
        {
            UmiProgramConsole *before = console;
            CHECK(UmiProgramConsoleDestroy(&console) == UMI_STATUS_BUSY && console == before);
            CHECK(UmiProgramConsoleRun(console) == UMI_STATUS_INVALID_STATE);
        }
        CHECK(UmiProgramConsoleStop(console) == UMI_STATUS_OK);
    }
    CHECK(ConsoleTestWait(console, snapshot, 0));
    CHECK(umi_thread_join(thread, NULL) == UMI_STATUS_OK);
    umi_thread_destroy(thread);
    thread = NULL;
    CHECK(snapshot->completed && snapshot->started);
    if (strcmp(mode, "stop") == 0 || strcmp(mode, "busy") == 0)
        CHECK(snapshot->status == UMI_STATUS_CANCELLED);
    else if (strcmp(mode, "timeout") == 0)
        CHECK(snapshot->status == UMI_STATUS_TIMEOUT);
    else
    {
        CHECK(snapshot->status == UMI_STATUS_OK);
        CHECK(snapshot->exit_code == (strcmp(mode, "exit") == 0 ? 23 : 0));
        if (strcmp(mode, "large") == 0)
        {
            CHECK(snapshot->output_truncated && snapshot->output_bytes_seen > 140000U);
            CHECK(snapshot->diagnostics_truncated);
            CHECK(strstr(snapshot->output, "last stdout") != NULL);
            CHECK(strstr(snapshot->diagnostics, "last stderr") != NULL);
        }
        else if (strcmp(mode, "environment") == 0)
            CHECK(strstr(snapshot->output, "value=caf\xc3\xa9 value") != NULL);
        else if (strcmp(mode, "exit") != 0)
        {
            CHECK(strstr(snapshot->output, "EOF") != NULL &&
                  strstr(snapshot->diagnostics, "finished input") != NULL);
            if (strcmp(mode, "eof") != 0)
            {
                CHECK(strstr(snapshot->output,
                             strcmp(mode, "copy") == 0 ? "owned input\n" : message) != NULL);
                CHECK(snapshot->input_bytes_sent == message_length);
            }
            else
                CHECK(snapshot->input_bytes_sent == 0U);
            if (strcmp(mode, "binary") == 0)
                CHECK(strstr(snapshot->output, "a?b") != NULL);
        }
    }
    CHECK(UmiProgramConsoleSend(console, "x", 1U) == UMI_STATUS_INVALID_STATE);
cleanup:
    ConsoleTestCleanup(&console, &thread);
    free(snapshot);
    return failed;
}
