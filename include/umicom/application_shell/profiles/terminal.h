/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/profiles/terminal.h
 *
 * PURPOSE:
 *   Publish the reusable Terminal shell profile for thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_PROFILE_TERMINAL_H
#define UMICOM_APPLICATION_SHELL_PROFILE_TERMINAL_H

#include "umicom/application_shell/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiApplicationShellProfileDefinition *umi_application_shell_profile_terminal(void);

#ifdef __cplusplus
}
#endif

#endif
