/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/profiles/sqls.h
 *
 * PURPOSE:
 *   Publish the built-in SQL Language Server language-server profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PROFILE_SQLS_H
#define UMICOM_LANGUAGE_RUNTIME_PROFILE_SQLS_H
#include "umicom/language/server_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime profile sqls operation used by this module and its client
 * applications.
 */
const UmiLanguageServerProfile *umi_language_runtime_profile_sqls(void);
#ifdef __cplusplus
}
#endif
#endif
