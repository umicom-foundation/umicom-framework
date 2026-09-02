/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/profiles/project_wizard.h
 *
 * PURPOSE:
 *   Publish the reusable Project Wizard shell profile for thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_PROFILE_PROJECT_WIZARD_H
#define UMICOM_APPLICATION_SHELL_PROFILE_PROJECT_WIZARD_H

#include "umicom/application_shell/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application shell profile project wizard operation used by this module and
 * its client applications.
 */
const UmiApplicationShellProfileDefinition *umi_application_shell_profile_project_wizard(void);

#ifdef __cplusplus
}
#endif

#endif
