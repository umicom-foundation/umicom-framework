/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/builtin_profiles.h
 *
 * PURPOSE:
 *   Provide deterministic enumeration and bulk installation of all Framework
 *   built-in shell profiles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_BUILTIN_PROFILES_H
#define UMICOM_APPLICATION_SHELL_BUILTIN_PROFILES_H

#include "umicom/application_shell/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_application_shell_builtin_profile_count(void);

const UmiApplicationShellProfileDefinition *
umi_application_shell_builtin_profile_at(size_t index);

const UmiApplicationShellProfileDefinition *
umi_application_shell_builtin_profile_find(const char *profile_id);

UmiStatus umi_application_shell_builtin_profiles_install(
    UmiApplicationShellRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
