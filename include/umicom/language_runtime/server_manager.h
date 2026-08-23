/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/server_manager.h
 *
 * PURPOSE:
 *   Own language servers per language/workspace and perform initialize/shutdown lifecycle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiLanguageRuntimeServerManager UmiLanguageRuntimeServerManager;
UmiStatus umi_language_runtime_server_manager_create(UmiLanguageService*l,UmiLanguageRuntimeServerManager**out);
void umi_language_runtime_server_manager_destroy(UmiLanguageRuntimeServerManager*m);
UmiStatus umi_language_runtime_server_manager_start_for_language(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root,const char*wd,uint32_t timeout,UmiLanguageRuntimeServer**out);
UmiStatus umi_language_runtime_server_manager_attach(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root,UmiLanguageRuntimeServer*s,const UmiLanguageRuntimeInitializeResult*caps);
UmiLanguageRuntimeServer *umi_language_runtime_server_manager_find(UmiLanguageRuntimeServerManager*m,const char*lang,const char*root);
UmiStatus umi_language_runtime_server_manager_stop_all(UmiLanguageRuntimeServerManager*m,uint32_t timeout);
size_t umi_language_runtime_server_manager_count(const UmiLanguageRuntimeServerManager*m);
#ifdef __cplusplus
}
#endif
#endif
