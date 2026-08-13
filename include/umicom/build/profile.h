/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/profile.h
 *
 * PURPOSE:
 *   Define and validate one reusable source, build, compiler and generator profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_PROFILE_H
#define UMICOM_BUILD_PROFILE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/build/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBuildProfile {
    char profile_id[UMI_BUILD_ID_CAPACITY];
    char source_directory[UMI_BUILD_PATH_CAPACITY];
    char build_directory[UMI_BUILD_PATH_CAPACITY];
    char generator[UMI_BUILD_NAME_CAPACITY];
    char compiler[UMI_BUILD_PATH_CAPACITY];
    char configuration[UMI_BUILD_NAME_CAPACITY];
    char preset[UMI_BUILD_NAME_CAPACITY];
    char build_target[UMI_BUILD_NAME_CAPACITY];
    char run_program[UMI_BUILD_PATH_CAPACITY];
    char run_argument[UMI_BUILD_ARGUMENT_CAPACITY];
    unsigned parallel_jobs;
    uint32_t timeout_ms;
    int build_testing;
    int strict_warnings;
} UmiBuildProfile;

void umi_build_profile_init(UmiBuildProfile *profile);
UmiStatus umi_build_profile_set(UmiBuildProfile *profile,
                                const char *profile_id,
                                const char *source_directory,
                                const char *build_directory);
UmiStatus umi_build_profile_validate(const UmiBuildProfile *profile,
                                     char *out_message,
                                     size_t message_capacity);
int umi_build_profile_equal(const UmiBuildProfile *left,
                            const UmiBuildProfile *right);

#ifdef __cplusplus
}
#endif

#endif
