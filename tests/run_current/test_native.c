/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_current/test_native.c
 * PURPOSE: Launch a real controlled executable with no generated CMake project or build directory.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiBuildProfile *profile = malloc(sizeof *profile);
    CHECK(profile != NULL);
    FixtureProfile(profile);
    CHECK(strlen(argv[1]) < sizeof profile->run_program);
    strcpy(profile->run_program, argv[1]);
    strcpy(profile->run_environment, "UMICOM_RUN_VALUE='caf\xc3\xa9 with spaces'");
    strcpy(profile->build_directory, "build-must-not-be-created");
    profile->run_arguments[0] = '\0';
    UmiBuildProjectSessionConfig config = {0};
    UmiBuildProjectSession *session = NULL;
    CHECK(umi_build_project_session_create(&config, &session) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(profile->build_directory));
    CHECK(UmiBuildProjectSessionRunCurrent(session, profile, true, NULL) == UMI_STATUS_OK);
    UmiBuildProjectSessionSnapshot state = FixtureWait(session);
    CHECK(state.status == UMI_STATUS_OK && state.completed_phase_count == 1U);
    UmiBuildResult *result = malloc(sizeof *result);
    CHECK(result != NULL);
    CHECK(umi_build_project_session_result_at(session, 0U, result) == UMI_STATUS_OK);
    CHECK(result->exit_code == 0 &&
          strstr(result->output, "received:[caf\xc3\xa9 with spaces]") != NULL);
    CHECK(!umi_fs_exists(profile->build_directory));
    free(result);
    free(profile);
    umi_build_project_session_destroy(session);
    return 0;
}
