/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/profiles/build_dashboard.h
 *
 * PURPOSE:
 *   Publish the reusable Build Dashboard shell profile for thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_PROFILE_BUILD_DASHBOARD_H
#define UMICOM_APPLICATION_SHELL_PROFILE_BUILD_DASHBOARD_H

#include "umicom/application_shell/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application shell profile build dashboard operation used by this module and
 * its client applications.
 */
const UmiApplicationShellProfileDefinition *umi_application_shell_profile_build_dashboard(void);

#ifdef __cplusplus
}
#endif

#endif
