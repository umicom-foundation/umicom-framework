/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/profiles/problems.h
 *
 * PURPOSE:
 *   Publish the reusable Problems shell profile for thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_PROFILE_PROBLEMS_H
#define UMICOM_APPLICATION_SHELL_PROFILE_PROBLEMS_H

#include "umicom/application_shell/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application shell profile problems operation used by this module and its
 * client applications.
 */
const UmiApplicationShellProfileDefinition *umi_application_shell_profile_problems(void);

#ifdef __cplusplus
}
#endif

#endif
