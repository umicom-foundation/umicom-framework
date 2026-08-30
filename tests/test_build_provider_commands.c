/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_provider_commands.c
 *
 * PURPOSE:
 *   Verify preset-aware configure/test commands, selected build targets,
 *   bounded parallelism and run-program construction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/build/cmake_provider.h"
#include "umicom/build/ctest_provider.h"

int main(void)
{
    UmiBuildProfile profile;
    UmiBuildProvider cmake = umi_build_cmake_provider();
    UmiBuildProvider ctest = umi_build_ctest_provider();
    UmiBuildCommand command;
    umi_build_profile_init(&profile);
    (void)strcpy(profile.preset, "windows-ucrt64-headless-debug");
    (void)strcpy(profile.build_target, "umicom-studio-ide");
    profile.parallel_jobs = 128U;

    assert(umi_build_provider_create_command(
               &cmake, &profile, UMI_BUILD_PHASE_CONFIGURE, &command) ==
           UMI_STATUS_OK);
    assert(command.argument_count == 2U);
    assert(strcmp(command.arguments[0], "--preset") == 0);
    assert(strcmp(command.arguments[1], profile.preset) == 0);

    assert(umi_build_provider_create_command(
               &cmake, &profile, UMI_BUILD_PHASE_BUILD, &command) ==
           UMI_STATUS_OK);
    assert(strcmp(command.arguments[2], "--target") == 0);
    assert(strcmp(command.arguments[3], profile.build_target) == 0);
    assert(strcmp(command.arguments[5], "32") == 0);

    assert(umi_build_provider_create_command(
               &ctest, &profile, UMI_BUILD_PHASE_TEST, &command) ==
           UMI_STATUS_OK);
    assert(strcmp(command.arguments[0], "--preset") == 0);

    (void)strcpy(profile.run_program, "umicom-studio-console");
    (void)strcpy(profile.run_argument, "--version");
    assert(umi_build_provider_create_command(
               &cmake, &profile, UMI_BUILD_PHASE_RUN, &command) ==
           UMI_STATUS_OK);
    assert(strcmp(command.program, profile.run_program) == 0);
    assert(command.argument_count == 1U);

    (void)strcpy(profile.install_directory, "build/stage");
    assert(umi_build_provider_create_command(
               &cmake, &profile, UMI_BUILD_PHASE_INSTALL, &command) ==
           UMI_STATUS_OK);
    assert(strcmp(command.arguments[0], "--install") == 0);
    assert(strcmp(command.arguments[1], profile.build_directory) == 0);
    assert(strcmp(command.arguments[2], "--prefix") == 0);
    assert(strcmp(command.arguments[3], profile.install_directory) == 0);
    return 0;
}
