/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/platform.h
 *
 * PURPOSE:
 *   Compose Language Service, Editor Session, persistent servers, document sync, async requests and dispatcher into the Framework IntelliSense runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * Represent the language runtime platform data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimePlatform UmiLanguageRuntimePlatform;
/**
 * Represent the language runtime platform snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimePlatformSnapshot{UmiLanguageServiceSnapshot language;UmiEditorSessionSnapshot editor;size_t server_count,open_document_count,pending_context_count;UmiLanguageRuntimeDispatchState dispatch;uint64_t revision;}UmiLanguageRuntimePlatformSnapshot;
/**
 * Initialise language runtime platform from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_runtime_platform_create(UmiLanguageRuntimePlatform**out);
/**
 * Release or reset state held by language runtime platform so the same storage can be
 * reused safely.
 */
void umi_language_runtime_platform_destroy(UmiLanguageRuntimePlatform*p);
/**
 * Provide the language runtime platform attach server operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_attach_server(UmiLanguageRuntimePlatform*p,const char*lang,const char*root,UmiLanguageRuntimeServer*s,const UmiLanguageRuntimeInitializeResult*caps);
/**
 * Provide the language runtime platform open document operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_open_document(UmiLanguageRuntimePlatform*p,const char*root,const char*wd,const char*d,const char*uri,const char*lang,const char*title,const char*text,uint32_t timeout);
/**
 * Provide the language runtime platform change document operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_change_document(UmiLanguageRuntimePlatform*p,const char*d,const char*text);
/**
 * Provide the language runtime platform save document operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_save_document(UmiLanguageRuntimePlatform*p,const char*d,const char*text);
/**
 * Provide the language runtime platform close document operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_close_document(UmiLanguageRuntimePlatform*p,const char*d);
/**
 * Provide the language runtime platform request completion operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_completion(UmiLanguageRuntimePlatform*p,const char*d,uint32_t line,uint32_t col);
/**
 * Provide the language runtime platform request hover operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_request_hover(UmiLanguageRuntimePlatform*p,const char*d,uint32_t line,uint32_t col);
/**
 * Provide the language runtime platform request signature operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_signature(UmiLanguageRuntimePlatform*p,const char*d,uint32_t line,uint32_t col);
/**
 * Provide the language runtime platform request definition operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_definition(UmiLanguageRuntimePlatform*p,const char*d,const char*symbol,uint32_t line,uint32_t col);
/**
 * Provide the language runtime platform request references operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_references(UmiLanguageRuntimePlatform*p,const char*d,const char*symbol,uint32_t line,uint32_t col,int include_decl);
/**
 * Provide the language runtime platform request document symbols operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_document_symbols(UmiLanguageRuntimePlatform*p,const char*d);
/**
 * Provide the language runtime platform request workspace symbols operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_workspace_symbols(UmiLanguageRuntimePlatform*p,const char*lang,const char*root,const char*query);
/**
 * Provide the language runtime platform request code actions operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_code_actions(UmiLanguageRuntimePlatform*p,const char*d,const UmiLanguageRuntimeRange*r);
/**
 * Provide the language runtime platform request formatting operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_formatting(UmiLanguageRuntimePlatform*p,const char*d,uint32_t tab,int spaces);
/**
 * Provide the language runtime platform request rename operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_request_rename(UmiLanguageRuntimePlatform*p,const char*d,const char*symbol,const char*old_name,const char*new_name,uint32_t line,uint32_t col);
/**
 * Provide the language runtime platform request semantic tokens operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_semantic_tokens(UmiLanguageRuntimePlatform*p,const char*d);
/**
 * Provide the language runtime platform request inlay hints operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_inlay_hints(UmiLanguageRuntimePlatform*p,const char*d,const UmiLanguageRuntimeRange*r);
/**
 * Provide the language runtime platform request folding ranges operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_platform_request_folding_ranges(UmiLanguageRuntimePlatform*p,const char*d);
/**
 * Provide the language runtime platform pump document operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_platform_pump_document(UmiLanguageRuntimePlatform*p,const char*d,uint32_t timeout,int*handled);
/**
 * Provide the language runtime platform pump server operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_platform_pump_server(UmiLanguageRuntimePlatform*p,const char*lang,const char*root,uint32_t timeout,int*handled);
/**
 * Provide the language runtime platform snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_platform_snapshot(UmiLanguageRuntimePlatform*p,UmiLanguageRuntimePlatformSnapshot*out);
/**
 * Provide the language runtime platform language operation used by this module and its
 * client applications.
 */
UmiLanguageService *umi_language_runtime_platform_language(UmiLanguageRuntimePlatform*p);
/**
 * Provide the language runtime platform editor operation used by this module and its
 * client applications.
 */
UmiEditorSession *umi_language_runtime_platform_editor(UmiLanguageRuntimePlatform*p);
/**
 * Provide the language runtime platform servers operation used by this module and its
 * client applications.
 */
UmiLanguageRuntimeServerManager *umi_language_runtime_platform_servers(UmiLanguageRuntimePlatform*p);
/**
 * Provide the language runtime platform documents operation used by this module and its
 * client applications.
 */
UmiLanguageRuntimeDocumentSync *umi_language_runtime_platform_documents(UmiLanguageRuntimePlatform*p);
/**
 * Provide the language runtime platform dispatch state operation used by this module and
 * its client applications.
 */
const UmiLanguageRuntimeDispatchState *umi_language_runtime_platform_dispatch_state(const UmiLanguageRuntimePlatform*p);
#ifdef __cplusplus
}
#endif
#endif
