/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/program_console/test_queue.c
 * PURPOSE: Check atomic input acceptance, EOF and controller misuse without starting a child.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    const char *mode = argv[1];
    int failed = 0;
    UmiProgramConsole *console = NULL;
    UmiProgramConsoleSnapshot *snapshot = calloc(1U, sizeof *snapshot);
    CHECK(snapshot != NULL);
    CHECK(ConsoleTestCreate(argv[2], "echo", 5000U, NULL, &console) == UMI_STATUS_OK);
    if (strcmp(mode, "capacity") == 0)
    {
        unsigned char bytes[UMI_PROGRAM_CONSOLE_INPUT_CAPACITY];
        memset(bytes, 'q', sizeof bytes);
        for (size_t i = 0U; i < UMI_PROGRAM_CONSOLE_QUEUE_CAPACITY; ++i)
            CHECK(UmiProgramConsoleSend(console, bytes, sizeof bytes) == UMI_STATUS_OK);
        CHECK(UmiProgramConsoleSend(console, "x", 1U) == UMI_STATUS_BUSY);
        CHECK(UmiProgramConsoleRead(console, snapshot) == UMI_STATUS_OK);
        CHECK(snapshot->queued_frames == UMI_PROGRAM_CONSOLE_QUEUE_CAPACITY &&
              snapshot->input_bytes_sent == 0U);
    }
    else if (strcmp(mode, "eof") == 0)
    {
        CHECK(UmiProgramConsoleSend(console, "accepted", 8U) == UMI_STATUS_OK);
        CHECK(UmiProgramConsoleEndInput(console) == UMI_STATUS_OK);
        CHECK(UmiProgramConsoleEndInput(console) == UMI_STATUS_OK);
        CHECK(UmiProgramConsoleSend(console, "rejected", 8U) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiProgramConsoleRead(console, snapshot) == UMI_STATUS_OK);
        CHECK(snapshot->input_ended && snapshot->queued_frames == 1U);
    }
    else if (strcmp(mode, "stop") == 0)
    {
        CHECK(UmiProgramConsoleSend(console, "queued", 6U) == UMI_STATUS_OK);
        CHECK(UmiProgramConsoleStop(console) == UMI_STATUS_OK);
        CHECK(UmiProgramConsoleSend(console, "x", 1U) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiProgramConsoleRun(console) == UMI_STATUS_CANCELLED);
        CHECK(UmiProgramConsoleRead(console, snapshot) == UMI_STATUS_OK);
        CHECK(!snapshot->started && snapshot->completed && snapshot->queued_frames == 0U);
        CHECK(UmiProgramConsoleRun(console) == UMI_STATUS_INVALID_STATE);
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        CHECK(UmiProgramConsoleSend(NULL, "x", 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProgramConsoleSend(console, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProgramConsoleSend(console, "", 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProgramConsoleSend(console, "x", 4097U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiProgramConsoleRead(console, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProgramConsoleRead(console, snapshot) == UMI_STATUS_OK &&
              snapshot->queued_frames == 0U);
    }
    else if (strcmp(mode, "request") == 0)
    {
        UmiProgramConsoleConfig config = {0};
        UmiProgramConsole *rejected = (UmiProgramConsole *)(uintptr_t)1U;
        CHECK(UmiProgramConsoleCreate(&config, &rejected) == UMI_STATUS_INVALID_ARGUMENT &&
              rejected == NULL);
        config.command.program = "relative";
        config.command.workingDirectory = "relative";
        CHECK(UmiProgramConsoleCreate(&config, &rejected) == UMI_STATUS_INVALID_ARGUMENT &&
              rejected == NULL);
    }
    else
    {
        failed = 2;
    }
cleanup:
    (void)UmiProgramConsoleDestroy(&console);
    free(snapshot);
    return failed;
}
