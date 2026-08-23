/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/terminal_profiles/wsl.h
 *
 * PURPOSE:
 *   Publish the Windows Subsystem for Linux terminal profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_TERMINAL_PROFILE_WSL_H
#define UMICOM_DEVELOPER_TERMINAL_PROFILE_WSL_H

#include "umicom/developer_productivity/terminal_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperTerminalProfile *umi_developer_terminal_profile_wsl(void);

#ifdef __cplusplus
}
#endif

#endif
