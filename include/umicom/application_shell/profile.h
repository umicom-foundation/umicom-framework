/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/profile.h
 *
 * PURPOSE:
 *   Define reusable shell profiles that contribute coherent groups of menus,
 *   views, panels, dashboards, palettes and commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_PROFILE_H
#define UMICOM_APPLICATION_SHELL_PROFILE_H

#include "umicom/application_shell/registry.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application shell profile definition data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationShellProfileDefinition {
    uint32_t structure_size;
    uint32_t api_version;
    const char *profile_id;
    const char *title;
    const char *description;
    const UmiApplicationShellContribution *contributions;
    size_t contribution_count;
} UmiApplicationShellProfileDefinition;

/**
 * Check that application shell profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_shell_profile_validate(
    const UmiApplicationShellProfileDefinition *profile);

/**
 * Provide the application shell profile install operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_shell_profile_install(
    UmiApplicationShellRegistry *registry,
    const UmiApplicationShellProfileDefinition *profile);

#ifdef __cplusplus
}
#endif

#endif
