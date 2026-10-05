/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/profile.h
 *
 * PURPOSE:
 *   Define and validate one reusable source, build, compiler and generator profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_PROFILE_H
#define UMICOM_BUILD_PROFILE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/base/arguments.h"
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
    char install_directory[UMI_BUILD_PATH_CAPACITY];
    unsigned parallel_jobs;
    uint32_t timeout_ms;
    int build_testing;
    int strict_warnings;
    /* Optional quoted argument list. Empty preserves run_argument as one literal
     * value. Fill only one field: conflicting launch inputs are rejected, never
     * silently combined. Appending preserves existing member offsets; consumers
     * must rebuild when using the enlarged public profile record. */
    char run_arguments[UMI_ARGUMENT_TEXT_CAPACITY];
    /* Independent stage names allow projects whose configure, build and test
     * presets differ. Leave preset empty when using these fields. An empty
     * stage uses the ordinary profile fields; no name is guessed from another
     * stage. Explicit presets own their options, except Clean's target and
     * the test policy requiring failures to be shown and tests to exist.
     * The original preset field keeps its configure/test behaviour for existing
     * callers. Appending retains previous member offsets; rebuild consumers
     * when adopting this expanded value record. */
    char configure_preset[UMI_BUILD_NAME_CAPACITY];
    char build_preset[UMI_BUILD_NAME_CAPACITY];
    char test_preset[UMI_BUILD_NAME_CAPACITY];
    /* Empty launches from the project root. A relative path is resolved from
     * that root; an absolute path selects a separate data directory. Run and
     * native Debug share this setting rather than inheriting the IDE's cwd. */
    char run_working_directory[UMI_BUILD_PATH_CAPACITY];
} UmiBuildProfile;

/* Resolve the effective argument vector used by both Run and Debug. Each
 * argument must fit the build command's 512-byte cell including its terminator.
 * Failure clears out. The profile and output must not overlap. */
UmiStatus UmiBuildProfileArguments(const UmiBuildProfile *profile, UmiArguments *out);

/** Resolve the launch folder without touching the filesystem or process cwd.
 * absoluteProjectDirectory must be the already resolved project root. The
 * output is changed only on success; it must not overlap either input. This
 * does not prove that the directory exists or grant permission to run code. */
UmiStatus UmiBuildProfileLaunchDirectory(const UmiBuildProfile *profile,
    const char *absoluteProjectDirectory, char *outDirectory, size_t capacity);

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
