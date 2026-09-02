/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/terminal_profile.h
 *
 * PURPOSE:
 *   Describe familiar shell profiles without hard-coding them in Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_PROFILE_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_PROFILE_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer terminal profile data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperTerminalProfile {
    uint32_t structure_size;
    uint32_t api_version;
    const char *profile_id;
    const char *title;
    const char *program;
    const char *const *arguments;
    size_t argument_count;
    const char *icon_id;
    int windows_only;
    int posix_only;
    int login_shell;
} UmiDeveloperTerminalProfile;

/**
 * Check that developer terminal profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_developer_terminal_profile_validate(
    const UmiDeveloperTerminalProfile *profile);

#ifdef __cplusplus
}
#endif

#endif
