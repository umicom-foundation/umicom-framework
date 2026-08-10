/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/cmake_provider.c
 *
 * PURPOSE:
 *   Create deterministic CMake configure, build and clean command records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/cmake_provider.h"

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
    umi_build_command_init(out_command, "cmake");
    if (phase == UMI_BUILD_PHASE_CONFIGURE) {
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
        (void)snprintf(jobs, sizeof(jobs), "%u", profile->parallel_jobs);
        if (!umi_build_command_add_argument(out_command, "--parallel") ||
            !umi_build_command_add_argument(out_command, jobs)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
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
        UMI_BUILD_PHASE_MASK(UMI_BUILD_PHASE_CLEAN);
    provider.create_command = cmake_command;
    return provider;
}
