/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/builtin_profiles.h
 *
 * PURPOSE:
 *   Provide deterministic enumeration and bulk installation of all Framework
 *   built-in shell profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_BUILTIN_PROFILES_H
#define UMICOM_APPLICATION_SHELL_BUILTIN_PROFILES_H

#include "umicom/application_shell/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the number of records represented by application shell builtin profile without
 * changing their state.
 */
size_t umi_application_shell_builtin_profile_count(void);

/**
 * Find application shell builtin profile while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationShellProfileDefinition *
umi_application_shell_builtin_profile_at(size_t index);

/**
 * Find application shell builtin profile while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationShellProfileDefinition *
umi_application_shell_builtin_profile_find(const char *profile_id);

/**
 * Provide the application shell builtin profiles install operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_shell_builtin_profiles_install(
    UmiApplicationShellRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
