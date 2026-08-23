/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/platform.h
 *
 * PURPOSE:
 *   Compose Language Service, Editor Session, persistent servers, document sync, async requests and dispatcher into the Framework IntelliSense runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_PLATFORM_H
#define UMICOM_LANGUAGE_RUNTIME_PLATFORM_H
#include "umicom/editor/session.h"
#include "umicom/language/service.h"
#include "umicom/language_runtime/dispatcher.h"
#include "umicom/language_runtime/server_manager.h"
#include "umicom/language_runtime/requests/requests.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiLanguageRuntimePlatform UmiLanguageRuntimePlatform;
typedef struct UmiLanguageRuntimePlatformSnapshot{UmiLanguageServiceSnapshot language;UmiEditorSessionSnapshot editor;size_t server_count,open_document_count,pending_context_count;UmiLanguageRuntimeDispatchState dispatch;uint64_t revision;}UmiLanguageRuntimePlatformSnapshot;
UmiStatus umi_language_runtime_platform_create(UmiLanguageRuntimePlatform**out);
void umi_language_runtime_platform_destroy(UmiLanguageRuntimePlatform*p);
UmiStatus umi_language_runtime_platform_attach_server(UmiLanguageRuntimePlatform*p,const char*lang,const char*root,UmiLanguageRuntimeServer*s,const UmiLanguageRuntimeInitializeResult*caps);
UmiStatus umi_language_runtime_platform_open_document(UmiLanguageRuntimePlatform*p,const char*root,const char*wd,const char*d,const char*uri,const char*lang,const char*title,const char*text,uint32_t timeout);
UmiStatus umi_language_runtime_platform_change_document(UmiLanguageRuntimePlatform*p,const char*d,const char*text);
UmiStatus umi_language_runtime_platform_save_document(UmiLanguageRuntimePlatform*p,const char*d,const char*text);
UmiStatus umi_language_runtime_platform_close_document(UmiLanguageRuntimePlatform*p,const char*d);
UmiStatus umi_language_runtime_platform_request_completion(UmiLanguageRuntimePlatform*p,const char*d,uint32_t line,uint32_t col);
UmiStatus umi_language_runtime_platform_request_hover(UmiLanguageRuntimePlatform*p,const char*d,uint32_t line,uint32_t col);
UmiStatus umi_language_runtime_platform_request_signature(UmiLanguageRuntimePlatform*p,const char*d,uint32_t line,uint32_t col);
UmiStatus umi_language_runtime_platform_request_definition(UmiLanguageRuntimePlatform*p,const char*d,const char*symbol,uint32_t line,uint32_t col);
UmiStatus umi_language_runtime_platform_request_references(UmiLanguageRuntimePlatform*p,const char*d,const char*symbol,uint32_t line,uint32_t col,int include_decl);
UmiStatus umi_language_runtime_platform_request_document_symbols(UmiLanguageRuntimePlatform*p,const char*d);
UmiStatus umi_language_runtime_platform_request_workspace_symbols(UmiLanguageRuntimePlatform*p,const char*lang,const char*root,const char*query);
UmiStatus umi_language_runtime_platform_request_code_actions(UmiLanguageRuntimePlatform*p,const char*d,const UmiLanguageRuntimeRange*r);
UmiStatus umi_language_runtime_platform_request_formatting(UmiLanguageRuntimePlatform*p,const char*d,uint32_t tab,int spaces);
UmiStatus umi_language_runtime_platform_request_rename(UmiLanguageRuntimePlatform*p,const char*d,const char*symbol,const char*old_name,const char*new_name,uint32_t line,uint32_t col);
UmiStatus umi_language_runtime_platform_request_semantic_tokens(UmiLanguageRuntimePlatform*p,const char*d);
UmiStatus umi_language_runtime_platform_request_inlay_hints(UmiLanguageRuntimePlatform*p,const char*d,const UmiLanguageRuntimeRange*r);
UmiStatus umi_language_runtime_platform_request_folding_ranges(UmiLanguageRuntimePlatform*p,const char*d);
UmiStatus umi_language_runtime_platform_pump_document(UmiLanguageRuntimePlatform*p,const char*d,uint32_t timeout,int*handled);
UmiStatus umi_language_runtime_platform_pump_server(UmiLanguageRuntimePlatform*p,const char*lang,const char*root,uint32_t timeout,int*handled);
UmiStatus umi_language_runtime_platform_snapshot(UmiLanguageRuntimePlatform*p,UmiLanguageRuntimePlatformSnapshot*out);
UmiLanguageService *umi_language_runtime_platform_language(UmiLanguageRuntimePlatform*p);
UmiEditorSession *umi_language_runtime_platform_editor(UmiLanguageRuntimePlatform*p);
UmiLanguageRuntimeServerManager *umi_language_runtime_platform_servers(UmiLanguageRuntimePlatform*p);
UmiLanguageRuntimeDocumentSync *umi_language_runtime_platform_documents(UmiLanguageRuntimePlatform*p);
const UmiLanguageRuntimeDispatchState *umi_language_runtime_platform_dispatch_state(const UmiLanguageRuntimePlatform*p);
#ifdef __cplusplus
}
#endif
#endif
