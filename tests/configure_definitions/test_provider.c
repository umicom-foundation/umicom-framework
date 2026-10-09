/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/configure_definitions/test_provider.c
 * PURPOSE: Verify configure-only option placement, preset overrides and durable job identity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/build/cmake_provider.h"
#include "umicom/build/job_history.h"
static size_t Count(const UmiBuildCommand *command, const char *text)
{
    size_t count = 0U;
    for (size_t index = 0U; index < command->argument_count; ++index)
        if (strcmp(command->arguments[index], text) == 0)
            ++count;
    return count;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiBuildProfile profile, original;
    Profile(&profile);
    original = profile;
    strcpy(profile.configure_definitions,
           "-DPROJECT_FEATURE:BOOL=ON -DCMAKE_PREFIX_PATH=\"C:/SDK Files\"");
    UmiBuildCommand *command = calloc(1U, sizeof *command);
    CHECK(command != NULL);
    UmiBuildProvider provider = umi_build_cmake_provider();
    const char *mode = argv[1];
    if (strcmp(mode, "identity") == 0)
    {
        UmiJobIdentity before, after, restored;
        CHECK(!umi_build_profile_equal(&profile, &original));
        CHECK(UmiBuildProfileJobIdentity(&original, &before) == UMI_STATUS_OK);
        CHECK(UmiBuildProfileJobIdentity(&profile, &after) == UMI_STATUS_OK);
        CHECK(strcmp(before.subject, after.subject) == 0 &&
              strcmp(before.configuration, after.configuration) != 0);
        profile.configure_definitions[0] = '\0';
        CHECK(UmiBuildProfileJobIdentity(&profile, &restored) == UMI_STATUS_OK);
        CHECK(strcmp(before.configuration, restored.configuration) == 0);
    }
    else if (strcmp(mode, "other-phases") == 0)
    {
        strcpy(profile.run_program, "build/notes");
        const UmiBuildPhase phases[] = {UMI_BUILD_PHASE_BUILD, UMI_BUILD_PHASE_CLEAN,
                                        UMI_BUILD_PHASE_INSTALL, UMI_BUILD_PHASE_DEPLOY,
                                        UMI_BUILD_PHASE_RUN};
        for (size_t index = 0U; index < sizeof phases / sizeof phases[0]; ++index)
        {
            CHECK(umi_build_provider_create_command(&provider, &profile, phases[index], command) ==
                  UMI_STATUS_OK);
            CHECK(Count(command, "-DPROJECT_FEATURE:BOOL=ON") == 0U);
        }
    }
    else
    {
        if (strcmp(mode, "preset") == 0)
            strcpy(profile.configure_preset, "reviewed-configure");
        else if (strcmp(mode, "shared-preset") == 0)
            strcpy(profile.preset, "reviewed-configure");
        else
            CHECK(strcmp(mode, "ordinary") == 0);
        CHECK(umi_build_provider_create_command(&provider, &profile, UMI_BUILD_PHASE_CONFIGURE,
                                                command) == UMI_STATUS_OK);
        CHECK(command->argument_count >= 4U);
        CHECK(strcmp(command->arguments[command->argument_count - 2U],
                     "-DPROJECT_FEATURE:BOOL=ON") == 0);
        CHECK(strcmp(command->arguments[command->argument_count - 1U],
                     "-DCMAKE_PREFIX_PATH=C:/SDK Files") == 0);
        CHECK(Count(command, "-DPROJECT_FEATURE:BOOL=ON") == 1U);
        if (strcmp(mode, "ordinary") != 0)
        {
            CHECK(command->argument_count == 4U && strcmp(command->arguments[0], "--preset") == 0);
            CHECK(strcmp(command->arguments[1], "reviewed-configure") == 0);
        }
        else
            CHECK(Count(command, "-DBUILD_TESTING=ON") == 1U &&
                  Count(command, "-DCMAKE_BUILD_TYPE=Debug") == 1U);
    }
    free(command);
    return 0;
}
