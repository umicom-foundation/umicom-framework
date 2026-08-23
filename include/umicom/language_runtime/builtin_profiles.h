/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/builtin_profiles.h
 *
 * PURPOSE:
 *   Enumerate/register built-in language-server profiles and select by language identity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_BUILTIN_PROFILES_H
#define UMICOM_LANGUAGE_RUNTIME_BUILTIN_PROFILES_H
#include "umicom/language/service.h"
#include "umicom/language_runtime/language_id.h"
#ifdef __cplusplus
extern "C" {
#endif
size_t umi_language_runtime_builtin_profile_count(void);
const UmiLanguageServerProfile *umi_language_runtime_builtin_profile_at(size_t index);
const UmiLanguageServerProfile *umi_language_runtime_builtin_profile_find(const char*id);
const UmiLanguageServerProfile *umi_language_runtime_builtin_profile_for_language(const char*language_id);
UmiStatus umi_language_runtime_register_builtin_profiles(UmiLanguageService*language);
#ifdef __cplusplus
}
#endif
#endif
