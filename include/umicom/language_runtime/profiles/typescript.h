/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/profiles/typescript.h
 *
 * PURPOSE:
 *   Publish the built-in TypeScript Language Server language-server profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PROFILE_TYPESCRIPT_H
#define UMICOM_LANGUAGE_RUNTIME_PROFILE_TYPESCRIPT_H
#include "umicom/language/server_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime profile typescript operation used by this module and its
 * client applications.
 */
const UmiLanguageServerProfile *umi_language_runtime_profile_typescript(void);
#ifdef __cplusplus
}
#endif
#endif
