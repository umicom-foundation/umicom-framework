/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/terminal_profiles/powershell.h
 *
 * PURPOSE:
 *   Publish the PowerShell terminal profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_TERMINAL_PROFILE_POWERSHELL_H
#define UMICOM_DEVELOPER_TERMINAL_PROFILE_POWERSHELL_H

#include "umicom/developer_productivity/terminal_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer terminal profile powershell operation used by this module and its
 * client applications.
 */
const UmiDeveloperTerminalProfile *umi_developer_terminal_profile_powershell(void);

#ifdef __cplusplus
}
#endif

#endif
