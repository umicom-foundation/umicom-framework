/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/operational_state/test_stage_presets.c
 * PURPOSE: Check independent CMake stage selection without launching a process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/cmake_provider.h"
#include "umicom/build/ctest_provider.h"
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

/* Inspect argument cells, not rendered shell text: spaces and punctuation
 * inside a preset name must remain one argument to CMake or CTest. */
static int Has(const UmiBuildCommand *command, const char *value)
{
    for (size_t i = 0U; i < command->argument_count; ++i)
        if (strcmp(command->arguments[i], value) == 0)
            return 1;
    return 0;
}
static int Is(const UmiBuildCommand *command, const char *program, size_t count, const char *const *arguments)
{
    if (strcmp(command->program, program) != 0 || command->argument_count != count)
        return 0;
    for (size_t i = 0U; i < count; ++i)
        if (strcmp(command->arguments[i], arguments[i]) != 0)
            return 0;
    return 1;
}
static void Profile(UmiBuildProfile *profile)
{
    umi_build_profile_init(profile);
    strcpy(profile->configure_preset, "notes-configure");
    strcpy(profile->build_preset, "notes-build");
    strcpy(profile->test_preset, "notes-test");
    /* These defaults must not override the explicit preset's own choices. */
    strcpy(profile->configuration, "RelWithDebInfo");
    strcpy(profile->build_target, "different-manual-target");
    profile->parallel_jobs = 7U;
}
int main(int argc, char **argv)
{
    UmiBuildProfile profile, changed;
    UmiBuildCommand command, before;
    UmiBuildProvider cmake = umi_build_cmake_provider();
    UmiBuildProvider ctest = umi_build_ctest_provider();
    CHECK(argc == 2);
    Profile(&profile);
    if (strcmp(argv[1], "configure") == 0)
    {
        const char *expected[] = {"--preset", "notes-configure"};
        CHECK(umi_build_provider_create_command(&cmake, &profile, UMI_BUILD_PHASE_CONFIGURE, &command) ==
              UMI_STATUS_OK);
        CHECK(Is(&command, "cmake", 2U, expected));
    }
    else if (strcmp(argv[1], "build") == 0)
    {
        const char *expected[] = {"--build", "--preset", "notes-build"};
        CHECK(umi_build_provider_create_command(&cmake, &profile, UMI_BUILD_PHASE_BUILD, &command) ==
              UMI_STATUS_OK);
        CHECK(Is(&command, "cmake", 3U, expected));
    }
    else if (strcmp(argv[1], "clean") == 0)
    {
        const char *expected[] = {"--build", "--preset", "notes-build", "--target", "clean"};
        CHECK(umi_build_provider_create_command(&cmake, &profile, UMI_BUILD_PHASE_CLEAN, &command) ==
              UMI_STATUS_OK);
        CHECK(Is(&command, "cmake", 5U, expected));
    }
    else if (strcmp(argv[1], "test") == 0)
    {
        const char *expected[] = {"--preset", "notes-test", "--output-on-failure", "--no-tests=error"};
        CHECK(umi_build_provider_create_command(&ctest, &profile, UMI_BUILD_PHASE_TEST, &command) ==
              UMI_STATUS_OK);
        CHECK(Is(&command, "ctest", 4U, expected));
    }
    else if (strcmp(argv[1], "fallback") == 0)
    {
        profile.build_preset[0] = '\0';
        profile.test_preset[0] = '\0';
        CHECK(umi_build_provider_create_command(&cmake, &profile, UMI_BUILD_PHASE_BUILD, &command) ==
              UMI_STATUS_OK);
        CHECK(!Has(&command, "--preset") && Has(&command, profile.build_directory));
        CHECK(Has(&command, profile.build_target) && Has(&command, "7"));
        CHECK(umi_build_provider_create_command(&ctest, &profile, UMI_BUILD_PHASE_TEST, &command) ==
              UMI_STATUS_OK);
        CHECK(!Has(&command, "--preset") && Has(&command, "--test-dir"));
        CHECK(Has(&command, profile.build_directory) && Has(&command, profile.configuration));
        profile.configure_preset[0] = '\0';
        strcpy(profile.build_preset, "notes-build");
        CHECK(umi_build_provider_create_command(&cmake, &profile, UMI_BUILD_PHASE_CONFIGURE, &command) ==
              UMI_STATUS_OK);
        CHECK(!Has(&command, "--preset") && Has(&command, "-S") && Has(&command, "-B"));
    }
    else if (strcmp(argv[1], "legacy") == 0)
    {
        umi_build_profile_init(&profile);
        strcpy(profile.preset, "shared-old-name");
        CHECK(umi_build_provider_create_command(&cmake, &profile, UMI_BUILD_PHASE_CONFIGURE, &command) ==
              UMI_STATUS_OK);
        CHECK(command.argument_count == 2U && Has(&command, profile.preset));
        CHECK(umi_build_provider_create_command(&cmake, &profile, UMI_BUILD_PHASE_BUILD, &command) ==
              UMI_STATUS_OK);
        CHECK(!Has(&command, "--preset") && Has(&command, profile.build_directory));
        CHECK(umi_build_provider_create_command(&ctest, &profile, UMI_BUILD_PHASE_TEST, &command) ==
              UMI_STATUS_OK);
        CHECK(Has(&command, profile.preset) && Has(&command, "--build-config"));
    }
    else if (strcmp(argv[1], "invalid") == 0)
    {
        strcpy(profile.preset, "conflicting-old-name");
        char message[256];
        CHECK(umi_build_profile_validate(&profile, message, sizeof(message)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strstr(message, "shared CMake preset") != NULL);
        memset(&command, 0x45, sizeof(command));
        before = command;
        CHECK(cmake.create_command(&profile, UMI_BUILD_PHASE_CONFIGURE, &command) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&command, &before, sizeof(command)) == 0);
        CHECK(ctest.create_command(&profile, UMI_BUILD_PHASE_TEST, &command) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&command, &before, sizeof(command)) == 0);
    }
    else if (strcmp(argv[1], "equal") == 0)
    {
        changed = profile;
        CHECK(umi_build_profile_equal(&profile, &changed));
        strcpy(changed.configure_preset, "another");
        CHECK(!umi_build_profile_equal(&profile, &changed));
        changed = profile;
        strcpy(changed.build_preset, "another");
        CHECK(!umi_build_profile_equal(&profile, &changed));
        changed = profile;
        strcpy(changed.test_preset, "another");
        CHECK(!umi_build_profile_equal(&profile, &changed));
    }
    else if (strcmp(argv[1], "install") == 0)
    {
        CHECK(umi_build_provider_create_command(&cmake, &profile, UMI_BUILD_PHASE_INSTALL, &command) ==
              UMI_STATUS_OK);
        CHECK(!Has(&command, "--preset") && Has(&command, "--install"));
        CHECK(Has(&command, profile.build_directory) && Has(&command, profile.install_directory));
        CHECK(umi_build_provider_create_command(&cmake, &profile, UMI_BUILD_PHASE_DEPLOY, &command) ==
              UMI_STATUS_OK);
        CHECK(!Has(&command, "--preset") && Has(&command, "--install"));
    }
    else if (strcmp(argv[1], "run") == 0)
    {
        strcpy(profile.run_program, "notes-program");
        strcpy(profile.run_arguments, "--file \"two words.txt\" \"\"");
        const char *expected[] = {"--file", "two words.txt", ""};
        CHECK(umi_build_provider_create_command(&cmake, &profile, UMI_BUILD_PHASE_RUN, &command) ==
              UMI_STATUS_OK);
        CHECK(Is(&command, "notes-program", 3U, expected));
    }
    else if (strcmp(argv[1], "names") == 0)
    {
        strcpy(profile.build_preset, "caf\xc3\xa9 notes;$HOME");
        CHECK(umi_build_provider_create_command(&cmake, &profile, UMI_BUILD_PHASE_BUILD, &command) ==
              UMI_STATUS_OK);
        CHECK(command.argument_count == 3U && strcmp(command.arguments[2], profile.build_preset) == 0);
    }
    else if (strcmp(argv[1], "boundary") == 0)
    {
        char *fields[] = {profile.configure_preset, profile.build_preset, profile.test_preset};
        for (size_t i = 0U; i < 3U; ++i)
        {
            memset(fields[i], 'x', UMI_BUILD_NAME_CAPACITY - 1U);
            fields[i][UMI_BUILD_NAME_CAPACITY - 1U] = '\0';
            CHECK(umi_build_profile_validate(&profile, NULL, 0U) == UMI_STATUS_OK);
            fields[i][UMI_BUILD_NAME_CAPACITY - 1U] = 'x';
            CHECK(umi_build_profile_validate(&profile, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
            fields[i][UMI_BUILD_NAME_CAPACITY - 1U] = '\0';
        }
    }
    else
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
