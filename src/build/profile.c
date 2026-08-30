/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/profile.c
 *
 * PURPOSE:
 *   Implement deterministic defaults, assignment and validation for build profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/profile.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    if (destination == NULL || source == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(source) + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, strlen(source) + 1U);
    return UMI_STATUS_OK;
}

void umi_build_profile_init(UmiBuildProfile *profile)
{
    if (profile == NULL) {
        return;
    }
    (void)memset(profile, 0, sizeof(*profile));
    (void)copy_text(profile->profile_id,
                    sizeof(profile->profile_id),
                    "default");
    (void)copy_text(profile->source_directory,
                    sizeof(profile->source_directory),
                    ".");
    (void)copy_text(profile->build_directory,
                    sizeof(profile->build_directory),
                    "build/default");
    (void)copy_text(profile->install_directory,
                    sizeof(profile->install_directory),
                    "build/default/install");
    (void)copy_text(profile->generator,
                    sizeof(profile->generator),
                    "Ninja");
    (void)copy_text(profile->configuration,
                    sizeof(profile->configuration),
                    "Debug");
    profile->parallel_jobs = 1U;
    profile->timeout_ms = 0U;
    profile->build_testing = 1;
    profile->strict_warnings = 1;
}

UmiStatus umi_build_profile_set(UmiBuildProfile *profile,
                                const char *profile_id,
                                const char *source_directory,
                                const char *build_directory)
{
    UmiStatus status;
    if (profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_build_profile_init(profile);
    status = copy_text(profile->profile_id,
                       sizeof(profile->profile_id),
                       profile_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(profile->source_directory,
                           sizeof(profile->source_directory),
                           source_directory);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(profile->build_directory,
                           sizeof(profile->build_directory),
                           build_directory);
    }
    return status;
}

UmiStatus umi_build_profile_validate(const UmiBuildProfile *profile,
                                     char *out_message,
                                     size_t message_capacity)
{
    const char *message = "Build profile is valid";
    UmiStatus status = UMI_STATUS_OK;
    if (profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (profile->profile_id[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build profile identifier is empty";
    } else if (profile->source_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build source directory is empty";
    } else if (profile->build_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build output directory is empty";
    } else if (profile->generator[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build generator is empty";
    } else if (profile->install_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build install directory is empty";
    } else if (profile->configuration[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build configuration is empty";
    } else if (profile->parallel_jobs == 0U) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Parallel job count must be greater than zero";
    }
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", message);
    }
    return status;
}

int umi_build_profile_equal(const UmiBuildProfile *left,
                            const UmiBuildProfile *right)
{
    if (left == NULL || right == NULL) {
        return 0;
    }
    return strcmp(left->profile_id, right->profile_id) == 0 &&
           strcmp(left->source_directory, right->source_directory) == 0 &&
           strcmp(left->build_directory, right->build_directory) == 0 &&
           strcmp(left->generator, right->generator) == 0 &&
           strcmp(left->compiler, right->compiler) == 0 &&
           strcmp(left->configuration, right->configuration) == 0 &&
           strcmp(left->preset, right->preset) == 0 &&
           strcmp(left->build_target, right->build_target) == 0 &&
           strcmp(left->run_program, right->run_program) == 0 &&
           strcmp(left->run_argument, right->run_argument) == 0 &&
           strcmp(left->install_directory, right->install_directory) == 0 &&
           left->parallel_jobs == right->parallel_jobs &&
           left->timeout_ms == right->timeout_ms &&
           left->build_testing == right->build_testing &&
           left->strict_warnings == right->strict_warnings;
}
