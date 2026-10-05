/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/operational_state/test_launch_directory.c
 * PURPOSE: Verify shared Run and Debug folder resolution without starting processes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/cmake_provider.h"
#include "umicom/build/ctest_provider.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(test)                                                                                          \
    do                                                                                                       \
    {                                                                                                        \
        if (!(test))                                                                                         \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test);                                       \
            return EXIT_FAILURE;                                                                             \
        }                                                                                                    \
    } while (0)
#ifdef _WIN32
static const char PROJECT[] = "C:/projects/Notes";
static const char OUTSIDE[] = "D:/test data";
#else
static const char PROJECT[] = "/projects/Notes";
static const char OUTSIDE[] = "/test data";
#endif
int main(int argc, char **argv)
{
    UmiBuildProfile profile, changed;
    char directory[UMI_BUILD_PATH_CAPACITY], expected[UMI_BUILD_PATH_CAPACITY];
    CHECK(argc == 2);
    umi_build_profile_init(&profile);
    strcpy(profile.source_directory, PROJECT);
    if (strcmp(argv[1], "default") == 0)
    {
        CHECK(UmiBuildProfileLaunchDirectory(&profile, PROJECT, directory, sizeof(directory)) ==
              UMI_STATUS_OK);
        CHECK(umi_path_equal(directory, PROJECT));
    }
    else if (strcmp(argv[1], "relative") == 0)
    {
        strcpy(profile.run_working_directory, "sample data/../caf\xc3\xa9");
        CHECK(UmiBuildProfileLaunchDirectory(&profile, PROJECT, directory, sizeof(directory)) ==
              UMI_STATUS_OK);
        CHECK(umi_path_join(PROJECT, "caf\xc3\xa9", expected, sizeof(expected)) == UMI_STATUS_OK);
        CHECK(umi_path_equal(directory, expected));
    }
    else if (strcmp(argv[1], "absolute") == 0)
    {
        strcpy(profile.run_working_directory, OUTSIDE);
        CHECK(UmiBuildProfileLaunchDirectory(&profile, PROJECT, directory, sizeof(directory)) ==
              UMI_STATUS_OK);
        CHECK(umi_path_equal(directory, OUTSIDE));
    }
    else if (strcmp(argv[1], "invalid") == 0)
    {
        strcpy(directory, "untouched");
        CHECK(UmiBuildProfileLaunchDirectory(&profile, "relative/project", directory, sizeof(directory)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(directory, "untouched") == 0);
        CHECK(UmiBuildProfileLaunchDirectory(&profile, PROJECT, directory, 2U) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(directory, "untouched") == 0);
        CHECK(UmiBuildProfileLaunchDirectory(NULL, PROJECT, directory, sizeof(directory)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildProfileLaunchDirectory(&profile, NULL, directory, sizeof(directory)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildProfileLaunchDirectory(&profile, PROJECT, NULL, sizeof(directory)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        memset(profile.run_working_directory, 'x', sizeof(profile.run_working_directory));
        CHECK(UmiBuildProfileLaunchDirectory(&profile, PROJECT, directory, sizeof(directory)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(directory, "untouched") == 0);
    }
    else if (strcmp(argv[1], "provider") == 0)
    {
        UmiBuildCommand command;
        UmiBuildProvider provider = umi_build_cmake_provider();
        strcpy(profile.run_program, "build/bin/notes");
        strcpy(profile.run_working_directory, "data");
        CHECK(UmiBuildProfileLaunchDirectory(&profile, PROJECT, expected, sizeof(expected)) == UMI_STATUS_OK);
        CHECK(umi_build_provider_create_command(&provider, &profile, UMI_BUILD_PHASE_RUN, &command) ==
              UMI_STATUS_OK);
        CHECK(umi_path_equal(command.working_directory, expected));
        CHECK(strcmp(command.program, profile.run_program) == 0);
        profile.run_working_directory[0] = '\0';
        CHECK(umi_build_provider_create_command(&provider, &profile, UMI_BUILD_PHASE_RUN, &command) ==
              UMI_STATUS_OK);
        CHECK(command.working_directory[0] == '\0'); /* Runner supplies the project-root default. */
    }
    else if (strcmp(argv[1], "phases") == 0)
    {
        UmiBuildCommand command;
        UmiBuildProvider provider = umi_build_cmake_provider(), ctest = umi_build_ctest_provider();
        strcpy(profile.run_working_directory, "data");
        const UmiBuildPhase phases[] = {UMI_BUILD_PHASE_CONFIGURE, UMI_BUILD_PHASE_BUILD,
                                        UMI_BUILD_PHASE_CLEAN, UMI_BUILD_PHASE_INSTALL};
        for (size_t i = 0U; i < sizeof(phases) / sizeof(phases[0]); ++i)
        {
            CHECK(umi_build_provider_create_command(&provider, &profile, phases[i], &command) ==
                  UMI_STATUS_OK);
            CHECK(command.working_directory[0] == '\0');
        }
        CHECK(umi_build_provider_create_command(&ctest, &profile, UMI_BUILD_PHASE_TEST, &command) ==
              UMI_STATUS_OK);
        CHECK(command.working_directory[0] == '\0');
    }
    else if (strcmp(argv[1], "equal") == 0)
    {
        changed = profile;
        strcpy(changed.run_working_directory, "data");
        CHECK(!umi_build_profile_equal(&profile, &changed));
        strcpy(profile.run_working_directory, "data");
        CHECK(umi_build_profile_equal(&profile, &changed));
    }
    else if (strcmp(argv[1], "drive-relative") == 0)
    {
#ifdef _WIN32
        const char *ambiguous[] = {"C:notes", "\\notes", "/notes"};
        for (size_t i = 0U; i < sizeof(ambiguous) / sizeof(ambiguous[0]); ++i)
        {
            strcpy(profile.run_working_directory, ambiguous[i]);
            CHECK(umi_build_profile_validate(&profile, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        }
        strcpy(profile.run_working_directory, "\\\\server\\share\\notes");
        CHECK(umi_build_profile_validate(&profile, NULL, 0U) == UMI_STATUS_OK);
#else
        return 77;
#endif
    }
    else
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
