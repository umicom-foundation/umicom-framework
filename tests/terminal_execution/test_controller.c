/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_execution/test_controller.c
 * PURPOSE: Check command dispatch, session identity and one-time terminal history publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../terminal_fixture.h"
#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    UmiTerminalTestFixture fixture;
    CHECK(terminal_fixture_create(&fixture) == UMI_STATUS_OK);
    CustomSession(fixture.controller);
    UmiTerminalCommand command;
    Command(&command, argv[2],
            strcmp(argv[1], "switch") == 0 || strcmp(argv[1], "destroy") == 0 ? "hold" : "success");
    char text[UMI_TERMINAL_COMMAND_CAPACITY];
    CHECK(umi_terminal_command_format(&command, text, sizeof text) == UMI_STATUS_OK);
    CHECK(UmiTerminalControllerArmBackground(fixture.controller) == UMI_STATUS_OK);
    CHECK(UmiTerminalControllerArmBackground(fixture.controller) == UMI_STATUS_BUSY);
    CHECK(UmiTerminalControllerBackgroundArmed(fixture.controller));
    if (strcmp(argv[1], "disarm") == 0)
    {
        UmiTerminalControllerDisarmBackground(fixture.controller);
        CHECK(!UmiTerminalControllerBackgroundArmed(fixture.controller));
        CHECK(!UmiTerminalControllerJobPending(fixture.controller));
        terminal_fixture_destroy(&fixture);
        return 0;
    }
    int exit_code = 123;
    CHECK(umi_terminal_controller_execute(fixture.controller, text, 0U, NULL, &exit_code) ==
          UMI_STATUS_OK);
    CHECK(exit_code == -1 && UmiTerminalControllerJobPending(fixture.controller));
    CHECK(!UmiTerminalControllerBackgroundArmed(fixture.controller));
    UmiTerminalSession *origin = umi_terminal_controller_active_session(fixture.controller);
    CHECK(umi_terminal_controller_close(fixture.controller, "fixture.job") == UMI_STATUS_BUSY);
    CHECK(umi_terminal_manager_close(fixture.manager, "fixture.job") == UMI_STATUS_BUSY);
    CHECK(umi_terminal_controller_execute(fixture.controller, text, 0U, NULL, NULL) ==
          UMI_STATUS_BUSY);
    if (strcmp(argv[1], "destroy") == 0)
    {
        (void)Until(origin, &fixture.clock, 1);
        umi_terminal_controller_destroy(fixture.controller);
        fixture.controller = NULL;
        CHECK(umi_process_supervisor_stats(fixture.supervisor).jobs == 0U);
        terminal_fixture_destroy(&fixture);
        return 0;
    }
    if (strcmp(argv[1], "switch") == 0)
    {
        (void)Until(origin, &fixture.clock, 1);
        CHECK(umi_terminal_controller_activate(fixture.controller, "fixture.primary") ==
              UMI_STATUS_OK);
        CHECK(UmiTerminalControllerStopJob(fixture.controller) == UMI_STATUS_OK);
    }
    UmiTerminalControllerExecution result;
    uint64_t start = fixture.clock.monotonic_nanoseconds(&fixture.clock);
    do
    {
        CHECK(UmiTerminalControllerPollJob(fixture.controller, &result) == UMI_STATUS_OK);
        CHECK(fixture.clock.monotonic_nanoseconds(&fixture.clock) - start < UINT64_C(8000000000));
        if (result.execution.pending)
            (void)fixture.clock.sleep_milliseconds(&fixture.clock, 10U);
    } while (result.execution.pending);
    CHECK(strcmp(result.session_id, "fixture.job") == 0);
    UmiTerminalHistory *history = umi_terminal_controller_history(fixture.controller);
    UmiTerminalHistoryEntry entry;
    CHECK(umi_terminal_history_at(history, 0U, &entry) == UMI_STATUS_OK);
    CHECK(strcmp(entry.session_id, "fixture.job") == 0 && entry.completed);
    CHECK(strcmp(entry.command, text) == 0);
    CHECK(UmiTerminalControllerPollJob(fixture.controller, &result) == UMI_STATUS_OK);
    CHECK(umi_terminal_history_at(history, 1U, &entry) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_process_supervisor_stats(fixture.supervisor).jobs == 0U);
    if (strcmp(argv[1], "switch") == 0)
    {
        CHECK(result.execution.process.state == UMI_PROCESS_JOB_CANCELLED);
        CHECK(strcmp(umi_terminal_tab_model_active_id(
                         umi_terminal_controller_tabs(fixture.controller)),
                     "fixture.primary") == 0);
        UmiTerminalSessionSnapshot primary;
        CHECK(umi_terminal_session_snapshot(
                  umi_terminal_controller_active_session(fixture.controller), &primary) ==
              UMI_STATUS_OK);
        CHECK(primary.commands_executed == 0U);
    }
    CHECK(umi_terminal_controller_close(fixture.controller, "fixture.job") == UMI_STATUS_OK);
    terminal_fixture_destroy(&fixture);
    return 0;
}
