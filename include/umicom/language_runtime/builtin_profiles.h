/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/builtin_profiles.h
 *
 * PURPOSE:
 *   Enumerate/register built-in language-server profiles and select by language identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_BUILTIN_PROFILES_H
#define UMICOM_LANGUAGE_RUNTIME_BUILTIN_PROFILES_H
#include "umicom/language/service.h"
#include "umicom/language_runtime/language_id.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Return the number of records represented by language runtime builtin profile without
 * changing their state.
 */
size_t umi_language_runtime_builtin_profile_count(void);
/**
 * Find language runtime builtin profile while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiLanguageServerProfile *umi_language_runtime_builtin_profile_at(size_t index);
/**
 * Find language runtime builtin profile while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiLanguageServerProfile *umi_language_runtime_builtin_profile_find(const char*id);
/**
 * Provide the language runtime builtin profile for language operation used by this module
 * and its client applications.
 */
const UmiLanguageServerProfile *umi_language_runtime_builtin_profile_for_language(const char*language_id);
/**
 * Provide the language runtime register builtin profiles operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_register_builtin_profiles(UmiLanguageService*language);
#ifdef __cplusplus
}
#endif
#endif
