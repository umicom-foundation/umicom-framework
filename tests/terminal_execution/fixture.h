/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_execution/fixture.h
 * PURPOSE: Keep supervised terminal assertions and deterministic request setup together.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_TERMINAL_EXECUTION_FIXTURE_H
#define UMICOM_TEST_TERMINAL_EXECUTION_FIXTURE_H
#include "umicom/platform/path.h"
#include "umicom/terminal/execution.h"
#include "umicom/terminal_ui/execution.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition)                                                                           \
    do                                                                                             \
    {                                                                                              \
        if (!(condition))                                                                          \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition);                        \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)

/* Build argv as owned fields, preserving spaces and backslashes literally. */
static inline void Command(UmiTerminalCommand *command, const char *program, const char *mode)
{
    umi_terminal_command_init(command);
    const char *values[] = {program, mode, "space value"};
    for (size_t index = 0U; index < 3U; ++index)
    {
        CHECK(strlen(values[index]) < sizeof command->argument_storage[index]);
        memcpy(command->argument_storage[index], values[index], strlen(values[index]) + 1U);
        command->arguments[index] = command->argument_storage[index];
    }
    command->argument_count = 3U;
}
static inline UmiTerminalExecutionSnapshot Until(UmiTerminalSession *session, UmiClock *clock,
                                                 int ready)
{
    uint64_t start = clock->monotonic_nanoseconds(clock);
    UmiTerminalExecutionSnapshot snapshot = {0};
    for (;;)
    {
        CHECK(UmiTerminalSessionPollJob(session, &snapshot) == UMI_STATUS_OK);
        if (ready ? strstr(snapshot.process.output, "ready:") != NULL : !snapshot.pending)
            return snapshot;
        CHECK(clock->monotonic_nanoseconds(clock) - start < UINT64_C(8000000000));
        CHECK(clock->sleep_milliseconds(clock, 10U) == UMI_STATUS_OK);
    }
}
static inline int TranscriptContains(UmiTerminalSession *session, const char *text)
{
    UmiTerminalTranscript *transcript = umi_terminal_session_transcript(session);
    for (size_t index = 0U; index < umi_terminal_transcript_count(transcript); ++index)
    {
        UmiTerminalTranscriptLine line;
        CHECK(umi_terminal_transcript_at(transcript, index, &line) == UMI_STATUS_OK);
        if (strstr(line.text, text) != NULL)
            return 1;
    }
    return 0;
}
static inline void CustomSession(UmiTerminalController *controller)
{
    UmiTerminalProfile profile;
    umi_terminal_profile_init(&profile);
    strcpy(profile.profile_id, "fixture.direct");
    strcpy(profile.title, "Direct fixture");
    strcpy(profile.program, "fixture");
    profile.kind = UMI_TERMINAL_PROFILE_CUSTOM;
    CHECK(umi_terminal_profile_registry_register(umi_terminal_controller_profiles(controller),
                                                 &profile) == UMI_STATUS_OK);
    CHECK(umi_terminal_controller_open(controller, profile.profile_id, "fixture.job", "Fixture job",
                                       ".") == UMI_STATUS_OK);
}
#endif
