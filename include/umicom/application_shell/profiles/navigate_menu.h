/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/profiles/navigate_menu.h
 *
 * PURPOSE:
 *   Publish the reusable Navigate Menu shell profile for thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_PROFILE_NAVIGATE_MENU_H
#define UMICOM_APPLICATION_SHELL_PROFILE_NAVIGATE_MENU_H

#include "umicom/application_shell/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application shell profile navigate menu operation used by this module and
 * its client applications.
 */
const UmiApplicationShellProfileDefinition *umi_application_shell_profile_navigate_menu(void);

#ifdef __cplusplus
}
#endif

#endif
