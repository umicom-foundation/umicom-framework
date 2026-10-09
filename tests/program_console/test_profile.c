/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/program_console/test_profile.c
 * PURPOSE: Check that console launches reuse project arguments and reject untrusted or ambiguous programs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/build/program_console.h"
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    const char *mode = argv[1];
    if (strcmp(mode, "launch") != 0 && strcmp(mode, "untrusted") != 0 &&
        strcmp(mode, "lookup") != 0 && strcmp(mode, "missing") != 0)
        return 2;
    int failed = 0;
    UmiBuildProfile *profile = calloc(1U, sizeof *profile);
    UmiProgramConsoleSnapshot *snapshot = calloc(1U, sizeof *snapshot);
    UmiProgramConsole *console = NULL;
    UmiThread *thread = NULL;
    char directory[UMI_PATH_CAPACITY];
    CHECK(profile != NULL && snapshot != NULL);
    CHECK(umi_path_parent(argv[2], directory, sizeof directory) == UMI_STATUS_OK);
    umi_build_profile_init(profile);
    CHECK(umi_build_profile_set(profile, "console-fixture", directory, "build") == UMI_STATUS_OK);
    CHECK(strlen(argv[2]) < sizeof profile->run_program);
    strcpy(profile->run_program, argv[2]);
    strcpy(profile->run_arguments, "environment");
    strcpy(profile->run_environment, "UMICOM_CONSOLE_FIXTURE_VALUE='profile value'");
    profile->timeout_ms = 5000U;
    if (strcmp(mode, "untrusted") == 0)
    {
        CHECK(UmiBuildProgramConsoleCreate(profile, false, &console) ==
                  UMI_STATUS_PERMISSION_DENIED &&
              console == NULL);
    }
    else if (strcmp(mode, "lookup") == 0)
    {
        strcpy(profile->run_program, "compiler");
        CHECK(UmiBuildProgramConsoleCreate(profile, true, &console) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              console == NULL);
    }
    else if (strcmp(mode, "missing") == 0)
    {
        strcpy(profile->run_program, "./missing-program-console-fixture");
        CHECK(UmiBuildProgramConsoleCreate(profile, true, &console) == UMI_STATUS_NOT_FOUND &&
              console == NULL);
    }
    else
    {
        CHECK(UmiBuildProgramConsoleCreate(profile, true, &console) == UMI_STATUS_OK);
        /* Configuration changes after creation cannot retarget the queued child. */
        strcpy(profile->run_arguments, "exit");
        strcpy(profile->run_environment, "UMICOM_CONSOLE_FIXTURE_VALUE=changed");
        CHECK(umi_thread_start(ConsoleTestWorker, console, &thread) == UMI_STATUS_OK);
        CHECK(ConsoleTestWait(console, snapshot, 0));
        CHECK(umi_thread_join(thread, NULL) == UMI_STATUS_OK);
        umi_thread_destroy(thread);
        thread = NULL;
        CHECK(snapshot->status == UMI_STATUS_OK && snapshot->exit_code == 0);
        CHECK(strstr(snapshot->output, "value=profile value") != NULL);
    }
cleanup:
    ConsoleTestCleanup(&console, &thread);
    free(profile);
    free(snapshot);
    return failed;
}
