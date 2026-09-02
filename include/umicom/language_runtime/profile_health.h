/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/profile_health.h
 *
 * PURPOSE:
 *   Report whether a configured language-server executable is discoverable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PROFILE_HEALTH_H
#define UMICOM_LANGUAGE_RUNTIME_PROFILE_HEALTH_H
#include "umicom/language/server_profile.h"
#include "umicom/language_runtime/types.h"
#include "umicom/toolchain/discovery.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime profile health data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeProfileHealth{char profile_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];char executable[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];char resolved_path[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];int available;UmiStatus status;}UmiLanguageRuntimeProfileHealth;
/**
 * Provide the language runtime profile health probe operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_profile_health_probe(const UmiLanguageServerProfile*p,UmiLanguageRuntimeProfileHealth*out);
#ifdef __cplusplus
}
#endif
#endif
