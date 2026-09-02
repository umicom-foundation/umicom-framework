/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/terminal_profiles/bash.h
 *
 * PURPOSE:
 *   Publish the Bash terminal profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_TERMINAL_PROFILE_BASH_H
#define UMICOM_DEVELOPER_TERMINAL_PROFILE_BASH_H

#include "umicom/developer_productivity/terminal_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer terminal profile bash operation used by this module and its client
 * applications.
 */
const UmiDeveloperTerminalProfile *umi_developer_terminal_profile_bash(void);

#ifdef __cplusplus
}
#endif

#endif
