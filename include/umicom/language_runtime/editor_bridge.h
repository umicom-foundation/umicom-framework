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
typedef struct UmiLanguageRuntimeEditorBridge{UmiLanguageService*language;UmiEditorSession*editor;uint64_t revision;}UmiLanguageRuntimeEditorBridge;
UmiStatus umi_language_runtime_editor_bridge_init(UmiLanguageRuntimeEditorBridge*b,UmiLanguageService*l,UmiEditorSession*e);
UmiStatus umi_language_runtime_editor_bridge_sync_document(UmiLanguageRuntimeEditorBridge*b,const char*d,const char*title,uint64_t bytes);
UmiStatus umi_language_runtime_editor_bridge_sync_completion(UmiLanguageRuntimeEditorBridge*b,const char*d);
UmiStatus umi_language_runtime_editor_bridge_sync_diagnostics(UmiLanguageRuntimeEditorBridge*b,const char*d);
UmiStatus umi_language_runtime_editor_bridge_sync_symbols(UmiLanguageRuntimeEditorBridge*b,const char*d);
UmiStatus umi_language_runtime_editor_bridge_sync_code_actions(UmiLanguageRuntimeEditorBridge*b,const char*d);
#ifdef __cplusplus
}
#endif
#endif
