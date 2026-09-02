/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/profiles/build_menu.h
 *
 * PURPOSE:
 *   Publish the reusable Build Menu shell profile for thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_PROFILE_BUILD_MENU_H
#define UMICOM_APPLICATION_SHELL_PROFILE_BUILD_MENU_H

#include "umicom/application_shell/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application shell profile build menu operation used by this module and its
 * client applications.
 */
const UmiApplicationShellProfileDefinition *umi_application_shell_profile_build_menu(void);

#ifdef __cplusplus
}
#endif

#endif
