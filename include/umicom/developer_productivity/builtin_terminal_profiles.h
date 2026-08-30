/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/builtin_terminal_profiles.h
 *
 * PURPOSE:
 *   Enumerate familiar built-in terminal profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_BUILTIN_TERMINAL_PROFILES_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_BUILTIN_TERMINAL_PROFILES_H

#include "umicom/developer_productivity/terminal_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_developer_builtin_terminal_profile_count(void);

const UmiDeveloperTerminalProfile *
umi_developer_builtin_terminal_profile_at(size_t index);

const UmiDeveloperTerminalProfile *
umi_developer_builtin_terminal_profile_find(const char *profile_id);

#ifdef __cplusplus
}
#endif

#endif
