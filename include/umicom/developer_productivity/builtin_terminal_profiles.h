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

/**
 * Return the number of records represented by developer builtin terminal profile without
 * changing their state.
 */
size_t umi_developer_builtin_terminal_profile_count(void);

/**
 * Find developer builtin terminal profile while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperTerminalProfile *
umi_developer_builtin_terminal_profile_at(size_t index);

/**
 * Find developer builtin terminal profile while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperTerminalProfile *
umi_developer_builtin_terminal_profile_find(const char *profile_id);

#ifdef __cplusplus
}
#endif

#endif
