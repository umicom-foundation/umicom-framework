/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/profiles/jdtls.h
 *
 * PURPOSE:
 *   Publish the built-in Eclipse JDT Language Server language-server profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PROFILE_JDTLS_H
#define UMICOM_LANGUAGE_RUNTIME_PROFILE_JDTLS_H
#include "umicom/language/server_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiLanguageServerProfile *umi_language_runtime_profile_jdtls(void);
#ifdef __cplusplus
}
#endif
#endif
