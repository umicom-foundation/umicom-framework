/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/profiles/search.h
 *
 * PURPOSE:
 *   Publish the reusable Search shell profile for thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_PROFILE_SEARCH_H
#define UMICOM_APPLICATION_SHELL_PROFILE_SEARCH_H

#include "umicom/application_shell/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application shell profile search operation used by this module and its
 * client applications.
 */
const UmiApplicationShellProfileDefinition *umi_application_shell_profile_search(void);

#ifdef __cplusplus
}
#endif

#endif
