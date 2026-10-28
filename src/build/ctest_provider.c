/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/ctest_provider.c
 *
 * PURPOSE:
 *   Create a deterministic CTest command for a selected build profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/tool_location.h"
#include "umicom/build/ctest_provider.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <string.h>

static UmiStatus ctest_command(const UmiBuildProfile *profile,
                               UmiBuildPhase phase,
                               UmiBuildCommand *out_command)
{
    if (profile == NULL || out_command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (phase != UMI_BUILD_PHASE_TEST) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    UmiStatus validation = umi_build_profile_validate(profile, NULL, 0U);
    if (validation != UMI_STATUS_OK) return validation;
/* An explicit project tool folder selects this program directly. The original inherited-lookup call remains for review; empty tool folders preserve that behaviour. The previous implementation is retained for engineering review. */
#if 0
    umi_build_command_init(out_command, "ctest");
#endif
    char tool[UMI_BUILD_PATH_CAPACITY];
    UmiStatus tool_status = UmiBuildToolProgram(profile, "ctest", tool, sizeof tool);
    if (tool_status != UMI_STATUS_OK) return tool_status;
    umi_build_command_init(out_command, tool);
    /* An explicit test preset supplies its own configuration, environment,
     * filters and execution options. Keep the established visible-failure and
     * nonempty-suite policy without overriding the preset's configuration. */
    if (profile->test_preset[0] != '\0') {
        return umi_build_command_add_argument(out_command, "--preset") &&
               umi_build_command_add_argument(out_command, profile->test_preset) &&
               umi_build_command_add_argument(out_command, "--output-on-failure") &&
               umi_build_command_add_argument(out_command, "--no-tests=error")
            ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* An empty test tree is not a passed test run. */
    if (!umi_build_command_add_argument(out_command, "--no-tests=error") ||
        !umi_build_command_add_argument(out_command, "--build-config") ||
        !umi_build_command_add_argument(out_command, profile->configuration))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (profile->preset[0] != '\0') {
        if (!umi_build_command_add_argument(out_command, "--preset") ||
            !umi_build_command_add_argument(out_command, profile->preset) ||
            !umi_build_command_add_argument(out_command,
                                            "--output-on-failure")) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        return UMI_STATUS_OK;
    }
    if (!umi_build_command_add_argument(out_command, "--test-dir") ||
        !umi_build_command_add_argument(out_command,
                                        profile->build_directory) ||
        !umi_build_command_add_argument(out_command,
                                        "--output-on-failure")) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiBuildProvider umi_build_ctest_provider(void)
{
    UmiBuildProvider provider;
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.provider_id = "umicom.build.ctest";
    provider.supported_phases =
        UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_TEST);
    provider.create_command = ctest_command;
    return provider;
}

/* The build tree owns its generated driver. This avoids rebuilding Framework
 * merely because Studio is compiling a different project. Presets may select
 * another binary directory or custom filters, so their established semantics
 * remain authoritative until their resolved contract is available here. */
UmiStatus UmiBuildAutomaticTestDriver(const UmiBuildProfile *profile,
    char *driver, size_t capacity)
{
    if (profile == NULL || driver == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    if (profile->preset[0] || profile->configure_preset[0] ||
        profile->build_preset[0] || profile->test_preset[0])
        return UMI_STATUS_NOT_FOUND;
    const char *config = profile->configuration;
    for (const char *p = config; *p != '\0'; ++p)
        if (!( (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') ||
               (*p >= '0' && *p <= '9') || *p == '_' || *p == '-'))
            return UMI_STATUS_INVALID_ARGUMENT;
    char source[UMI_BUILD_PATH_CAPACITY], build[UMI_BUILD_PATH_CAPACITY];
    status = UmiBuildProfileSourceDirectory(profile, source, sizeof(source));
    if (status == UMI_STATUS_OK)
        status = UmiBuildProfileBuildDirectory(profile, source, build, sizeof(build));
    if (status != UMI_STATUS_OK) return status;
    int size = snprintf(driver, capacity, "%s/umicom/automatic-tests-%s.cmake", build, config);
    if (size < 0 || (size_t)size >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!umi_fs_is_file(driver)) return UMI_STATUS_NOT_FOUND;
    return UMI_STATUS_OK;
}

static UmiStatus AutomaticCommand(const UmiBuildProfile *profile,
    UmiBuildPhase phase, UmiBuildCommand *command)
{
    if (profile == NULL || command == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (phase != UMI_BUILD_PHASE_TEST) return UMI_STATUS_NOT_IMPLEMENTED;
    /* Shared discovery lets a worker delegate the entire operation once.
     * The original inline lookup is retained for architectural review. */
#if 0
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    if (profile->preset[0] || profile->configure_preset[0] ||
        profile->build_preset[0] || profile->test_preset[0])
        return ctest_command(profile, phase, command);
    const char *config = profile->configuration;
    for (const char *p = config; *p != '\0'; ++p)
        if (!( (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') ||
               (*p >= '0' && *p <= '9') || *p == '_' || *p == '-'))
            return UMI_STATUS_INVALID_ARGUMENT;
    char source[UMI_BUILD_PATH_CAPACITY], build[UMI_BUILD_PATH_CAPACITY];
    char driver[UMI_BUILD_PATH_CAPACITY], option[UMI_BUILD_ARGUMENT_CAPACITY];
    status = UmiBuildProfileSourceDirectory(profile, source, sizeof(source));
    if (status == UMI_STATUS_OK)
        status = UmiBuildProfileBuildDirectory(profile, source, build, sizeof(build));
    if (status != UMI_STATUS_OK) return status;
    int size = snprintf(driver, sizeof(driver), "%s/umicom/automatic-tests-%s.cmake", build, config);
    if (size < 0 || (size_t)size >= sizeof(driver)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!umi_fs_is_file(driver)) return ctest_command(profile, phase, command);
#endif
    char driver[UMI_BUILD_PATH_CAPACITY], option[UMI_BUILD_ARGUMENT_CAPACITY];
    UmiStatus status = UmiBuildAutomaticTestDriver(profile, driver, sizeof(driver));
    if (status == UMI_STATUS_NOT_FOUND) return ctest_command(profile, phase, command);
    if (status != UMI_STATUS_OK) return status;
    const char *config = profile->configuration;
    int size;
    char tool[UMI_BUILD_PATH_CAPACITY];
    status = UmiBuildToolProgram(profile, "cmake", tool, sizeof(tool));
    if (status != UMI_STATUS_OK) return status;
    size = snprintf(option, sizeof(option), "-DUMICOM_AUTOMATIC_CONFIGURATION=%s", config);
    if (size < 0 || (size_t)size >= sizeof(option)) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_build_command_init(command, tool);
    return umi_build_command_add_argument(command, option) &&
        umi_build_command_add_argument(command, "-P") &&
        umi_build_command_add_argument(command, driver) ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
UmiBuildProvider UmiBuildAutomaticTestProvider(void)
{
    UmiBuildProvider provider = umi_build_ctest_provider();
    provider.provider_id = "umicom.build.automatic-tests";
    provider.create_command = AutomaticCommand;
    return provider;
}
