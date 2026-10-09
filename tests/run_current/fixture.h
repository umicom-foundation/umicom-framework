/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_current/fixture.h
 * PURPOSE: Keep isolated launch fixtures and bounded wait helpers shared by the run-only tests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_RUN_CURRENT_TEST_FIXTURE_H
#define UMICOM_RUN_CURRENT_TEST_FIXTURE_H
#include "umicom/build/run_current.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)
static inline void FixtureProfile(UmiBuildProfile *profile)
{
    umi_build_profile_init(profile);
    CHECK(umi_fs_current_directory(profile->source_directory, sizeof profile->source_directory) ==
          UMI_STATUS_OK);
    CHECK(umi_fs_write_text("selected-program", "controlled executor fixture\n") == UMI_STATUS_OK);
    strcpy(profile->run_program, "./selected-program");
    strcpy(profile->run_working_directory, ".");
    strcpy(profile->run_arguments, "'argument with spaces'");
    strcpy(profile->run_environment, "UMICOM_RUN_VALUE=selected");
}
static inline UmiBuildProjectSessionSnapshot FixtureWait(UmiBuildProjectSession *session)
{
    UmiBuildProjectSessionSnapshot state;
    for (unsigned attempt = 0U; attempt < 5000U; ++attempt)
    {
        CHECK(umi_build_project_session_snapshot(session, &state) == UMI_STATUS_OK);
        if (!state.active)
            return state;
        umi_thread_sleep_ms(1U);
    }
    CHECK(0);
    memset(&state, 0, sizeof state);
    return state;
}
#endif
