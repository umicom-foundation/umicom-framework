/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_execution/test_directory.c
 * PURPOSE: Keep terminal directory changes explicit, local and independent of other sessions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/platform/filesystem.h"
#include "umicom/terminal/location.h"

int main(int argc, char **argv)
{
    CHECK(argc == 3);
    char before[UMI_PATH_CAPACITY], after[UMI_PATH_CAPACITY], selected[UMI_PATH_CAPACITY];
    CHECK(umi_fs_current_directory(before, sizeof before) == UMI_STATUS_OK);
    if (strcmp(argv[1], "relative") == 0)
    {
        strcpy(selected, "sentinel");
        CHECK(UmiTerminalDirectorySelect(".", selected, sizeof selected) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(selected[0] == '\0');
    }
    else if (strcmp(argv[1], "missing") == 0)
    {
        char missing[UMI_PATH_CAPACITY];
        CHECK(umi_path_join(before, "absent-terminal-folder", missing, sizeof missing) ==
              UMI_STATUS_OK);
        CHECK(!umi_fs_exists(missing));
        CHECK(UmiTerminalDirectorySelect(missing, selected, sizeof selected) ==
              UMI_STATUS_NOT_FOUND);
        CHECK(!umi_fs_exists(missing));
    }
    else if (strcmp(argv[1], "capacity") == 0)
    {
        char tiny[1] = {'x'};
        CHECK(UmiTerminalDirectorySelect(before, tiny, sizeof tiny) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(tiny[0] == '\0');
    }
    else if (strcmp(argv[1], "alias") == 0)
    {
        strcpy(selected, before);
        CHECK(UmiTerminalDirectorySelect(selected, selected, sizeof selected) == UMI_STATUS_OK);
        CHECK(umi_path_equal(selected, before));
    }
    else
    {
        UmiClock clock = umi_clock_system();
        UmiTerminalSessionConfig config = {"first", "First", ".", 16U, &clock};
        UmiTerminalSession *first = NULL, *second = NULL;
        CHECK(umi_terminal_session_create(&config, &first) == UMI_STATUS_OK);
        config.session_id = "second";
        CHECK(umi_terminal_session_create(&config, &second) == UMI_STATUS_OK);
        if (strcmp(argv[1], "closed") == 0)
        {
            CHECK(umi_terminal_session_close(first) == UMI_STATUS_OK);
            CHECK(UmiTerminalSessionChooseDirectory(first, before) == UMI_STATUS_INVALID_STATE);
        }
        else if (strcmp(argv[1], "running") == 0)
        {
            UmiProcessSupervisor *supervisor = NULL;
            CHECK(umi_process_supervisor_create(NULL, &supervisor) == UMI_STATUS_OK);
            UmiTerminalCommand command;
            Command(&command, argv[2], "hold");
            CHECK(UmiTerminalSessionStartJob(first, supervisor, &command, "hold", 0U) ==
                  UMI_STATUS_OK);
            CHECK(UmiTerminalSessionChooseDirectory(first, before) == UMI_STATUS_BUSY);
            CHECK(UmiTerminalSessionStopJob(first) == UMI_STATUS_OK);
            CHECK(UmiTerminalSessionWaitJob(first, 8000U) == UMI_STATUS_OK);
            umi_process_supervisor_destroy(supervisor);
        }
        else
        {
            CHECK(strcmp(argv[1], "independent") == 0);
            CHECK(UmiTerminalSessionChooseDirectory(first, before) == UMI_STATUS_OK);
            UmiTerminalSessionSnapshot one, two;
            CHECK(umi_terminal_session_snapshot(first, &one) == UMI_STATUS_OK);
            CHECK(umi_terminal_session_snapshot(second, &two) == UMI_STATUS_OK);
            CHECK(umi_path_equal(one.working_directory, before));
            CHECK(strcmp(two.working_directory, ".") == 0);
            CHECK(UmiTerminalSessionChooseDirectory(first, "relative") ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_terminal_session_snapshot(first, &one) == UMI_STATUS_OK);
            CHECK(umi_path_equal(one.working_directory, before));
        }
        umi_terminal_session_destroy(second);
        umi_terminal_session_destroy(first);
        umi_clock_dispose(&clock);
    }
    CHECK(umi_fs_current_directory(after, sizeof after) == UMI_STATUS_OK);
    CHECK(umi_path_equal(before, after));
    return 0;
}
