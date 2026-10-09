/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/program_console/fixture.h
 * PURPOSE: Keep console regression setup bounded and independent of user projects.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROGRAM_CONSOLE_FIXTURE_H
#define UMICOM_PROGRAM_CONSOLE_FIXTURE_H
#include "umicom/platform/path.h"
#include "umicom/platform/threading.h"
#include "umicom/terminal/program_console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            failed = 1;                                                                            \
            goto cleanup;                                                                          \
        }                                                                                          \
    } while (0)
static inline int ConsoleTestWorker(void *context) { return (int)UmiProgramConsoleRun(context); }
static inline UmiStatus ConsoleTestCreate(const char *program, const char *mode, uint32_t timeout,
                                          const char *environment, UmiProgramConsole **out)
{
    char directory[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_parent(program, directory, sizeof directory);
    if (status != UMI_STATUS_OK)
        return status;
    UmiProgramConsoleConfig config = {0};
    config.command.program = program;
    config.command.arguments = &mode;
    config.command.argumentCount = 1U;
    config.command.workingDirectory = directory;
    config.timeout_ms = timeout;
    config.environment = environment;
    return UmiProgramConsoleCreate(&config, out);
}
static inline int ConsoleTestWait(UmiProgramConsole *console, UmiProgramConsoleSnapshot *snapshot,
                                  int started)
{
    for (unsigned i = 0U; i < 2000U; ++i)
    {
        if (UmiProgramConsoleRead(console, snapshot) != UMI_STATUS_OK)
            return 0;
        if (snapshot->completed || (started && snapshot->started))
            return 1;
        umi_thread_sleep_ms(5U);
    }
    return 0;
}
static inline void ConsoleTestCleanup(UmiProgramConsole **console, UmiThread **thread)
{
    if (*console != NULL)
        (void)UmiProgramConsoleStop(*console);
    if (*thread != NULL)
    {
        (void)umi_thread_join(*thread, NULL);
        umi_thread_destroy(*thread);
        *thread = NULL;
    }
    (void)UmiProgramConsoleDestroy(console);
}
#endif
