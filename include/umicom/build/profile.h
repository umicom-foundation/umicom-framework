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
    /* Optional absolute folder containing the project's CMake/CTest/CPack and
     * Ninja programs. The runner prepends it to PATH only in child processes,
     * including Run, so compiler helpers and runtime libraries can be found.
     * Empty retains inherited lookup. Append to preserve earlier member offsets;
     * rebuild consumers when adopting this enlarged public value type. */
    char tool_directory[UMI_BUILD_PATH_CAPACITY];
    /* Reviewed -D definitions are passed as literal configure arguments, including
     * after an explicit preset. Use them for SDK prefixes and project options.
     * Existing dedicated profile fields remain authoritative. Append this field
     * to retain earlier member offsets; consumers must rebuild the value type. */
    char configure_definitions[UMI_ARGUMENT_TEXT_CAPACITY];
    /* Explicit non-secret NAME=VALUE settings for Run and native Debug.
     * The host, configure/build/test processes and terminal sessions keep their
     * existing environment. These values are saved in project settings; use a
     * secret provider, not this record, for passwords or API credentials. */
    char run_environment[UMI_ARGUMENT_TEXT_CAPACITY];
} UmiBuildProfile;

/* Resolve the effective argument vector used by both Run and Debug. Each
 * argument must fit the build command's 512-byte cell including its terminator.
 * Failure clears out. The profile and output must not overlap. */
UmiStatus UmiBuildProfileArguments(const UmiBuildProfile *profile, UmiArguments *out);

/** Resolve the source directory at the caller's current location, copying an
 * absolute root only on success. An already absolute root never reads cwd.
 * Capturing this before queue submission keeps later phases on the same project
 * if another operation changes the process directory. This does not check
 * existence, canonicalise symlinks or grant workspace trust. */
UmiStatus UmiBuildProfileSourceDirectory(const UmiBuildProfile *profile, char *out, size_t capacity);

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
