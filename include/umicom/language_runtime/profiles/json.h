/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/profiles/json.h
 *
 * PURPOSE:
 *   Publish the built-in JSON Language Server language-server profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PROFILE_JSON_H
#define UMICOM_LANGUAGE_RUNTIME_PROFILE_JSON_H
#include "umicom/language/server_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime profile json operation used by this module and its client
 * applications.
 */
const UmiLanguageServerProfile *umi_language_runtime_profile_json(void);
#ifdef __cplusplus
}
#endif
#endif
