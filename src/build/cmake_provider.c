/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/cmake_provider.c
 *
 * PURPOSE:
 *   Create deterministic CMake configure, build and clean command records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/cmake_provider.h"
#include "umicom/build/policy.h"

#include <stdio.h>
#include <string.h>

static UmiStatus cmake_command(const UmiBuildProfile *profile,
                               UmiBuildPhase phase,
                               UmiBuildCommand *out_command)
{
    char definition[UMI_BUILD_ARGUMENT_CAPACITY];
    char jobs[64];
    if (profile == NULL || out_command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Validate value records at the provider boundary as well as in runners.
     * Direct callers must not bypass conflicting or unterminated preset checks. */
    UmiStatus validation = umi_build_profile_validate(profile, NULL, 0U);
    if (validation != UMI_STATUS_OK) return validation;
    umi_build_command_init(out_command, "cmake");
    if (phase == UMI_BUILD_PHASE_CONFIGURE) {
        /* CMake resolves preset inheritance, environment and binary directory.
         * Do not silently replace those reviewed settings with GUI defaults. */
        if (profile->configure_preset[0] != '\0') {
            return umi_build_command_add_argument(out_command, "--preset") &&
                   umi_build_command_add_argument(out_command, profile->configure_preset)
                ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (profile->preset[0] != '\0') {
            if (!umi_build_command_add_argument(out_command, "--preset") ||
                !umi_build_command_add_argument(out_command, profile->preset)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            return UMI_STATUS_OK;
        }
        if (!umi_build_command_add_argument(out_command, "-S") ||
            !umi_build_command_add_argument(out_command,
                                            profile->source_directory) ||
            !umi_build_command_add_argument(out_command, "-B") ||
            !umi_build_command_add_argument(out_command,
                                            profile->build_directory) ||
            !umi_build_command_add_argument(out_command, "-G") ||
            !umi_build_command_add_argument(out_command,
                                            profile->generator)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)snprintf(definition,
                       sizeof(definition),
                       "-DCMAKE_BUILD_TYPE=%s",
                       profile->configuration);
        if (!umi_build_command_add_argument(out_command, definition)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (profile->compiler[0] != '\0') {
            static const char prefix[] = "-DCMAKE_C_COMPILER=";
            size_t compiler_length = strlen(profile->compiler);
            if (sizeof(prefix) + compiler_length > sizeof(definition)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            (void)memcpy(definition, prefix, sizeof(prefix) - 1U);
            (void)memcpy(definition + sizeof(prefix) - 1U,
                         profile->compiler,
                         compiler_length + 1U);
            if (!umi_build_command_add_argument(out_command, definition)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        }
        (void)snprintf(definition,
                       sizeof(definition),
                       "-DBUILD_TESTING=%s",
                       profile->build_testing ? "ON" : "OFF");
        if (!umi_build_command_add_argument(out_command, definition)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)snprintf(definition,
                       sizeof(definition),
                       "-DUMICOM_ENABLE_STRICT_WARNINGS=%s",
                       profile->strict_warnings ? "ON" : "OFF");
        return umi_build_command_add_argument(out_command, definition)
            ? UMI_STATUS_OK
            : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (phase == UMI_BUILD_PHASE_BUILD ||
        phase == UMI_BUILD_PHASE_CLEAN) {
        if (profile->build_preset[0] != '\0') {
            if (!umi_build_command_add_argument(out_command, "--build") ||
                !umi_build_command_add_argument(out_command, "--preset") ||
                !umi_build_command_add_argument(out_command, profile->build_preset))
                return UMI_STATUS_CAPACITY_EXCEEDED;
            /* Clean is an explicit action. Override only its target; ordinary
             * Build honours the preset's targets, configuration and job count. */
            if (phase == UMI_BUILD_PHASE_CLEAN &&
                (!umi_build_command_add_argument(out_command, "--target") ||
                 !umi_build_command_add_argument(out_command, "clean")))
                return UMI_STATUS_CAPACITY_EXCEEDED;
            return UMI_STATUS_OK;
        }
        if (!umi_build_command_add_argument(out_command, "--build") ||
            !umi_build_command_add_argument(out_command,
                                            profile->build_directory)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (phase == UMI_BUILD_PHASE_CLEAN &&
            (!umi_build_command_add_argument(out_command, "--target") ||
             !umi_build_command_add_argument(out_command, "clean"))) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (phase == UMI_BUILD_PHASE_BUILD &&
            profile->build_target[0] != '\0' &&
            (!umi_build_command_add_argument(out_command, "--target") ||
             !umi_build_command_add_argument(out_command,
                                             profile->build_target))) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (!umi_build_command_add_argument(out_command, "--config") ||
            !umi_build_command_add_argument(out_command, profile->configuration)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)snprintf(jobs, sizeof(jobs), "%u",
                       umi_build_policy_safe_parallel_jobs(
                           profile->parallel_jobs, 0U, 0U));
        if (!umi_build_command_add_argument(out_command, "--parallel") ||
            !umi_build_command_add_argument(out_command, jobs)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        return UMI_STATUS_OK;
    }
    /* Former predicate: phase == UMI_BUILD_PHASE_INSTALL. Local Deploy uses
     * the same install command after the session has completed its tests. */
    if (phase == UMI_BUILD_PHASE_INSTALL || phase == UMI_BUILD_PHASE_DEPLOY) {
        if (!umi_build_command_add_argument(out_command, "--install") ||
            !umi_build_command_add_argument(out_command,
                                            profile->build_directory) ||
            !umi_build_command_add_argument(out_command, "--prefix") ||
            !umi_build_command_add_argument(out_command,
                                            profile->install_directory) ||
            !umi_build_command_add_argument(out_command, "--config") ||
            !umi_build_command_add_argument(out_command,
                                            profile->configuration)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        return UMI_STATUS_OK;
    }
    if (phase == UMI_BUILD_PHASE_RUN && profile->run_program[0] != '\0') {
        umi_build_command_init(out_command, profile->run_program);
        /* The runner has already resolved source_directory to the project
         * root. Only Run receives this folder; build tools still use the root
         * so they can find the project's presets. Empty keeps the old default. */
        if (profile->run_working_directory[0] != '\0') {
            UmiStatus directoryStatus = UmiBuildProfileLaunchDirectory(profile,
                profile->source_directory, out_command->working_directory,
                sizeof(out_command->working_directory));
            if (directoryStatus != UMI_STATUS_OK) return directoryStatus;
        }
        /* The profile resolver preserves old literal inputs and shares exact
         * argument boundaries with the debugger. The former single-argument
         * path is retained below for review. */
#if 0
        if (profile->run_argument[0] != '\0' &&
            !umi_build_command_add_argument(out_command,
                                            profile->run_argument)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
#endif
        UmiArguments arguments;
        UmiStatus status = UmiBuildProfileArguments(profile, &arguments);
        if (status != UMI_STATUS_OK) return status;
        for (size_t index = 0U; index < arguments.count; ++index)
            if (!umi_build_command_add_argument(out_command, arguments.values[index]))
                return UMI_STATUS_CAPACITY_EXCEEDED;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_IMPLEMENTED;
}

UmiBuildProvider umi_build_cmake_provider(void)
{
    UmiBuildProvider provider;
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.provider_id = "umicom.build.cmake";
    provider.supported_phases =
        UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_CONFIGURE) |
        UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_BUILD) |
        UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_CLEAN) |
        UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_RUN) |
        UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_INSTALL) |
        UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_DEPLOY);
    provider.create_command = cmake_command;
    return provider;
}

// MIGRATION REFERENCE — previous implementation excerpts
// The shared build session now distinguishes Package, Rebuild and local Deploy. Command execution stays in src/build/runner.c and its existing providers; CPack uses src/build/cpack_provider.c.
// These comments explain superseded statements; do not enable both execution paths.
// Previous source near line 119:
//     if (phase == UMI_BUILD_PHASE_INSTALL) {
// Previous source near line 155:
//         UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_INSTALL);
