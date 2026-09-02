/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/server_manager.h
 *
 * PURPOSE:
 *   Own language servers per language/workspace and perform initialize/shutdown lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SERVER_MANAGER_H
#define UMICOM_LANGUAGE_RUNTIME_SERVER_MANAGER_H
#include "umicom/language_runtime/builtin_profiles.h"
#include "umicom/language_runtime/profile_health.h"
#include "umicom/language_runtime/decoders/initialize.h"
#include "umicom/language_runtime/requests/initialize.h"
#include "umicom/language_runtime/requests/initialized.h"
#include "umicom/language_runtime/requests/shutdown.h"
#include "umicom/language_runtime/requests/exit.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime server manager data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeServerManager UmiLanguageRuntimeServerManager;
/**
 * Initialise language runtime server manager from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_runtime_server_manager_create(UmiLanguageService*l,UmiLanguageRuntimeServerManager**out);
/**
 * Release or reset state held by language runtime server manager so the same storage can
 * be reused safely.
 */
void umi_language_runtime_server_manager_destroy(UmiLanguageRuntimeServerManager*m);
/**
 * Provide the language runtime server manager start for language operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_server_manager_start_for_language(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root,const char*wd,uint32_t timeout,UmiLanguageRuntimeServer**out);
/**
 * Provide the language runtime server manager attach operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_server_manager_attach(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root,UmiLanguageRuntimeServer*s,const UmiLanguageRuntimeInitializeResult*caps);
/**
 * Find language runtime server manager while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiLanguageRuntimeServer *umi_language_runtime_server_manager_find(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root);
/**
 * Provide the language runtime server manager stop all operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_server_manager_stop_all(UmiLanguageRuntimeServerManager*m,uint32_t timeout);
/**
 * Return the number of records represented by language runtime server manager without
 * changing their state.
 */
size_t umi_language_runtime_server_manager_count(const UmiLanguageRuntimeServerManager*m);
#ifdef __cplusplus
}
#endif
#endif
