/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/profile.h
 *
 * PURPOSE:
 *   Define one validated, application-neutral terminal launch profile. Profiles
 *   describe a shell and its environment; they never mutate the parent process.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiTerminalProfileEnvironment {
    char name[128];
    char value[UMI_TERMINAL_PATH_CAPACITY];
} UmiTerminalProfileEnvironment;

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

void umi_terminal_profile_init(UmiTerminalProfile *profile);
UmiStatus umi_terminal_profile_validate(const UmiTerminalProfile *profile);
UmiStatus umi_terminal_profile_set_argument(UmiTerminalProfile *profile,
                                            size_t index,
                                            const char *argument);
UmiStatus umi_terminal_profile_set_environment(UmiTerminalProfile *profile,
                                               const char *name,
                                               const char *value);
UmiStatus umi_terminal_profile_build_command(const UmiTerminalProfile *profile,
                                             char *out_command,
                                             size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
