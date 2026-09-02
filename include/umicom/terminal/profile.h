/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/profile.h
 *
 * PURPOSE:
 *   Define one validated, application-neutral terminal launch profile. Profiles
 *   describe a shell and its environment; they never mutate the parent process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_PROFILE_H
#define UMICOM_TERMINAL_PROFILE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/terminal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TERMINAL_PROFILE_API_VERSION 1U

/**
 * Represent the terminal profile environment data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalProfileEnvironment {
    char name[128];
    char value[UMI_TERMINAL_PATH_CAPACITY];
} UmiTerminalProfileEnvironment;

/**
 * Represent the terminal profile data shared with callers of this public contract.
 */
typedef struct UmiTerminalProfile {
    uint32_t struct_size;
    uint32_t api_version;
    char profile_id[UMI_TERMINAL_ID_CAPACITY];
    char title[UMI_TERMINAL_TITLE_CAPACITY];
    char icon_name[UMI_TERMINAL_TITLE_CAPACITY];
    UmiTerminalProfileKind kind;
    char program[UMI_TERMINAL_PATH_CAPACITY];
    char argument_storage[UMI_TERMINAL_PROFILE_ARGUMENT_MAX]
                         [UMI_TERMINAL_ARGUMENT_CAPACITY];
    size_t argument_count;
    char working_directory[UMI_TERMINAL_PATH_CAPACITY];
    UmiTerminalProfileEnvironment environment[
        UMI_TERMINAL_PROFILE_ENVIRONMENT_MAX];
    size_t environment_count;
    int login_shell;
    int inherit_environment;
    int visible;
} UmiTerminalProfile;

/**
 * Initialise terminal profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_terminal_profile_init(UmiTerminalProfile *profile);
/**
 * Check that terminal profile satisfies its contract before another service relies on it.
 */
UmiStatus umi_terminal_profile_validate(const UmiTerminalProfile *profile);
/**
 * Provide the terminal profile set argument operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_profile_set_argument(UmiTerminalProfile *profile,
                                            size_t index,
                                            const char *argument);
/**
 * Provide the terminal profile set environment operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_profile_set_environment(UmiTerminalProfile *profile,
                                               const char *name,
                                               const char *value);
/**
 * Provide the terminal profile build command operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_profile_build_command(const UmiTerminalProfile *profile,
                                             char *out_command,
                                             size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
