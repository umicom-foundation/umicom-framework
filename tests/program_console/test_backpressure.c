/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/program_console/test_backpressure.c
 * PURPOSE: Exercise input saturation and child output closure without conflating stdout EOF with process exit.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    const bool blocked = strcmp(argv[1], "blocked-input") == 0;
    if (!blocked && strcmp(argv[1], "closed-output") != 0)
        return 2;
    int failed = 0;
    UmiProgramConsole *console = NULL;
    UmiThread *thread = NULL;
    UmiProgramConsoleSnapshot *snapshot = calloc(1U, sizeof *snapshot);
    CHECK(snapshot != NULL);
    CHECK(ConsoleTestCreate(argv[2], blocked ? "wait" : "closed-output", 15000U, NULL, &console) ==
          UMI_STATUS_OK);
    if (!blocked)
    {
        CHECK(UmiProgramConsoleSend(console, "still accepted", 14U) == UMI_STATUS_OK);
        CHECK(UmiProgramConsoleEndInput(console) == UMI_STATUS_OK);
    }
    CHECK(umi_thread_start(ConsoleTestWorker, console, &thread) == UMI_STATUS_OK);
    if (blocked)
    {
        unsigned char frame[UMI_PROGRAM_CONSOLE_INPUT_CAPACITY];
        memset(frame, 'x', sizeof frame);
        /* Keep presenting bounded frames until the native pipe fills. BUSY is
         * expected backpressure, never permission to discard or overwrite input. */
        for (unsigned attempt = 0U; attempt < 2000U; ++attempt)
        {
            CHECK(UmiProgramConsoleRead(console, snapshot) == UMI_STATUS_OK);
            if (snapshot->completed)
                break;
            UmiStatus status = UmiProgramConsoleSend(console, frame, sizeof frame);
            CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_BUSY ||
                  status == UMI_STATUS_INVALID_STATE);
            umi_thread_sleep_ms(5U);
        }
    }
    if (blocked)
    {
        CHECK(UmiProgramConsoleRead(console, snapshot) == UMI_STATUS_OK);
        CHECK(snapshot->completed); /* The 15-second job deadline must not be the escape route. */
    }
    CHECK(ConsoleTestWait(console, snapshot, 0));
    CHECK(umi_thread_join(thread, NULL) == UMI_STATUS_OK);
    umi_thread_destroy(thread);
    thread = NULL;
    if (blocked)
    {
        CHECK(snapshot->status == UMI_STATUS_TIMEOUT && snapshot->started);
        CHECK(snapshot->input_bytes_sent > 0U && snapshot->queued_frames == 0U);
    }
    else
    {
        CHECK(snapshot->status == UMI_STATUS_OK && snapshot->exit_code == 0);
        CHECK(strcmp(snapshot->output, "") == 0);
        CHECK(strstr(snapshot->diagnostics, "input bytes=14") != NULL);
        CHECK(UmiProgramConsoleEndInput(console) == UMI_STATUS_OK);
    }
cleanup:
    ConsoleTestCleanup(&console, &thread);
    free(snapshot);
    return failed;
}
