/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/profiles/status_bar.h
 *
 * PURPOSE:
 *   Publish the reusable Status Bar shell profile for thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_PROFILE_STATUS_BAR_H
#define UMICOM_APPLICATION_SHELL_PROFILE_STATUS_BAR_H

#include "umicom/application_shell/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application shell profile status bar operation used by this module and its
 * client applications.
 */
const UmiApplicationShellProfileDefinition *umi_application_shell_profile_status_bar(void);

#ifdef __cplusplus
}
#endif

#endif
