/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_execution/test_rejection.c
 * PURPOSE: Reject invalid terminal requests before changing session state or consuming process capacity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    UmiClock clock = umi_clock_system();
    UmiProcessSupervisor *supervisor = NULL;
    CHECK(umi_process_supervisor_create(NULL, &supervisor) == UMI_STATUS_OK);
    UmiTerminalSessionConfig config = {"fixture", "Fixture", ".", 16U, &clock};
    UmiTerminalSession *session = NULL;
    CHECK(umi_terminal_session_create(&config, &session) == UMI_STATUS_OK);
    UmiTerminalCommand command;
    Command(&command, argv[2], "success");
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(argv[1], "empty") == 0)
        command.argument_count = 0U;
    else if (strcmp(argv[1], "count") == 0)
        command.argument_count = UMI_TERMINAL_MAX_ARGUMENTS + 1U;
    else if (strcmp(argv[1], "pointer") == 0)
        command.arguments[1] = NULL;
    else if (strcmp(argv[1], "closed") == 0)
    {
        CHECK(umi_terminal_session_close(session) == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    else
        CHECK(0);
    UmiTerminalSessionSnapshot before, after;
    CHECK(umi_terminal_session_snapshot(session, &before) == UMI_STATUS_OK);
    CHECK(UmiTerminalSessionStartJob(session, supervisor, &command, "rejected", 0U) == expected);
    CHECK(umi_terminal_session_snapshot(session, &after) == UMI_STATUS_OK);
    CHECK(after.state == before.state && after.commands_executed == 0U);
    CHECK(after.transcript_lines == before.transcript_lines);
    CHECK(umi_process_supervisor_stats(supervisor).jobs == 0U);
    umi_terminal_session_destroy(session);
    umi_process_supervisor_destroy(supervisor);
    umi_clock_dispose(&clock);
    return 0;
}
