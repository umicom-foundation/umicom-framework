/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/terminal_profiles/cmd.h
 *
 * PURPOSE:
 *   Publish the Command Prompt terminal profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_TERMINAL_PROFILE_CMD_H
#define UMICOM_DEVELOPER_TERMINAL_PROFILE_CMD_H

#include "umicom/developer_productivity/terminal_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer terminal profile cmd operation used by this module and its client
 * applications.
 */
const UmiDeveloperTerminalProfile *umi_developer_terminal_profile_cmd(void);

#ifdef __cplusplus
}
#endif

#endif
