/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_execution/test_session.c
 * PURPOSE: Check captured output, cancellation, owned arguments and supervisor slot reuse.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1];
    UmiClock clock = umi_clock_system();
    UmiProcessSupervisor *supervisor = NULL;
    UmiProcessSupervisorConfig config = umi_process_supervisor_config_default();
    config.capacity = 1U;
    CHECK(umi_process_supervisor_create(&config, &supervisor) == UMI_STATUS_OK);
    UmiTerminalSessionConfig session_config = {"fixture", "Command fixture", ".", 16U, &clock};
    UmiTerminalSession *session = NULL;
    CHECK(umi_terminal_session_create(&session_config, &session) == UMI_STATUS_OK);
    UmiTerminalCommand command;
    int hold = strcmp(mode, "stop") == 0 || strcmp(mode, "timeout") == 0 ||
               strcmp(mode, "guards") == 0 || strcmp(mode, "destroy") == 0;
    Command(&command, argv[2],
            hold                         ? "hold"
            : strcmp(mode, "large") == 0 ? "large"
            : strcmp(mode, "exit") == 0  ? "fail"
                                         : "success");
    CHECK(umi_terminal_environment_set(umi_terminal_session_environment(session),
                                       "UMICOM_TERMINAL_FIXTURE", "captured") == UMI_STATUS_OK);
    CHECK(UmiTerminalSessionStartJob(session, supervisor, &command, "fixture command",
                                     strcmp(mode, "timeout") == 0 ? 500U : 0U) == UMI_STATUS_OK);
    /* Submission has copied argv and environment before either can be changed. */
    memset(&command, 0, sizeof command);
    CHECK(umi_terminal_environment_set(umi_terminal_session_environment(session),
                                       "UMICOM_TERMINAL_FIXTURE", "changed") == UMI_STATUS_OK);
    UmiTerminalExecutionSnapshot result;
    if (strcmp(mode, "guards") == 0)
    {
        Command(&command, argv[2], "success");
        CHECK(UmiTerminalSessionStartJob(session, supervisor, &command, "second", 0U) ==
              UMI_STATUS_BUSY);
        CHECK(umi_terminal_session_execute_prepared(session, &command, "second", 0U, NULL, NULL) ==
              UMI_STATUS_BUSY);
        CHECK(umi_terminal_session_execute(session, "\"broken", 0U, NULL, NULL) == UMI_STATUS_BUSY);
        CHECK(umi_terminal_session_set_working_directory(session, "..") == UMI_STATUS_BUSY);
        CHECK(umi_terminal_session_close(session) == UMI_STATUS_BUSY);
        CHECK(UmiTerminalSessionStopJob(session) == UMI_STATUS_OK);
    }
    else if (strcmp(mode, "stop") == 0 || strcmp(mode, "destroy") == 0)
    {
        result = Until(session, &clock, 1);
        CHECK(result.pending);
        if (strcmp(mode, "destroy") == 0)
        {
            umi_terminal_session_destroy(session);
            session = NULL;
        }
        else
            CHECK(UmiTerminalSessionStopJob(session) == UMI_STATUS_OK);
    }
    if (session != NULL)
    {
        result = Until(session, &clock, 0);
        CHECK(!result.pending);
        if (strcmp(mode, "stop") == 0 || strcmp(mode, "guards") == 0)
            CHECK(result.process.state == UMI_PROCESS_JOB_CANCELLED);
        else if (strcmp(mode, "timeout") == 0)
            CHECK(result.process.state == UMI_PROCESS_JOB_TIMED_OUT);
        else if (strcmp(mode, "exit") == 0)
            CHECK(result.process.exit_code == 9);
        else
        {
            CHECK(result.process.state == UMI_PROCESS_JOB_SUCCEEDED);
            CHECK(result.process.exit_code == 0);
            CHECK(strstr(result.process.output, "last:caf\xc3\xa9") != NULL);
            if (strcmp(mode, "large") == 0)
            {
                CHECK(result.process.output_truncated);
                CHECK(TranscriptContains(session, "truncated"));
            }
            else
            {
                CHECK(strstr(result.process.output, "ready:captured") != NULL);
                CHECK(strstr(result.process.output, "argument:space value") != NULL);
                CHECK(strstr(result.process.output, "ready:changed") == NULL);
            }
        }
        UmiTerminalSessionSnapshot before, after;
        CHECK(umi_terminal_session_snapshot(session, &before) == UMI_STATUS_OK);
        CHECK(before.commands_executed == 1U);
        size_t lines = umi_terminal_transcript_count(umi_terminal_session_transcript(session));
        CHECK(UmiTerminalSessionPollJob(session, &result) == UMI_STATUS_OK);
        CHECK(umi_terminal_session_snapshot(session, &after) == UMI_STATUS_OK);
        CHECK(after.commands_executed == before.commands_executed);
        CHECK(lines == umi_terminal_transcript_count(umi_terminal_session_transcript(session)));
        if (strcmp(mode, "reuse") == 0)
        {
            UmiProcessJobId first = result.process.job_id;
            Command(&command, argv[2], "success");
            CHECK(UmiTerminalSessionStartJob(session, supervisor, &command, "second", 0U) ==
                  UMI_STATUS_OK);
            result = Until(session, &clock, 0);
            CHECK(result.process.job_id != first);
            CHECK(umi_terminal_session_snapshot(session, &after) == UMI_STATUS_OK);
            CHECK(after.commands_executed == 2U);
        }
    }
    UmiProcessSupervisorStats stats = umi_process_supervisor_stats(supervisor);
    CHECK(stats.jobs == 0U);
    umi_terminal_session_destroy(session);
    umi_process_supervisor_destroy(supervisor);
    umi_clock_dispose(&clock);
    return 0;
}
