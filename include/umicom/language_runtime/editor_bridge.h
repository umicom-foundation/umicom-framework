/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/editor_bridge.h
 *
 * PURPOSE:
 *   Project Language Service documents/completion/diagnostics/symbols/actions into the existing Editor Session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_EDITOR_BRIDGE_H
#define UMICOM_LANGUAGE_RUNTIME_EDITOR_BRIDGE_H
#include "umicom/editor/session.h"
#include "umicom/language/service.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime editor bridge data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeEditorBridge{UmiLanguageService*language;UmiEditorSession*editor;uint64_t revision;}UmiLanguageRuntimeEditorBridge;
/**
 * Initialise language runtime editor bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_runtime_editor_bridge_init(UmiLanguageRuntimeEditorBridge*b,UmiLanguageService*l,UmiEditorSession*e);
/**
 * Provide the language runtime editor bridge sync document operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_editor_bridge_sync_document(UmiLanguageRuntimeEditorBridge*b,const char*d,const char*title,uint64_t bytes);
/**
 * Provide the language runtime editor bridge sync completion operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_editor_bridge_sync_completion(UmiLanguageRuntimeEditorBridge*b,const char*d);
/**
 * Provide the language runtime editor bridge sync diagnostics operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_editor_bridge_sync_diagnostics(UmiLanguageRuntimeEditorBridge*b,const char*d);
/**
 * Provide the language runtime editor bridge sync symbols operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_editor_bridge_sync_symbols(UmiLanguageRuntimeEditorBridge*b,const char*d);
/**
 * Provide the language runtime editor bridge sync code actions operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_runtime_editor_bridge_sync_code_actions(UmiLanguageRuntimeEditorBridge*b,const char*d);
#ifdef __cplusplus
}
#endif
#endif
