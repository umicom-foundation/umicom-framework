/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/service_bridge.h
 *
 * PURPOSE:
 *   Project decoded LSP results into the existing provider-neutral UmiLanguageService registries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SERVICE_BRIDGE_H
#define UMICOM_LANGUAGE_RUNTIME_SERVICE_BRIDGE_H
#include "umicom/language/service.h"
#include "umicom/language_runtime/decoders/decoders.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiLanguageRuntimeServiceBridge{UmiLanguageService*language;uint64_t revision;}UmiLanguageRuntimeServiceBridge;
UmiStatus umi_language_runtime_service_bridge_init(UmiLanguageRuntimeServiceBridge*b,UmiLanguageService*l);
UmiStatus umi_language_runtime_publish_completion(UmiLanguageRuntimeServiceBridge*b,const char*d,uint32_t line,uint32_t col,const UmiLanguageRuntimeCompletionResult*r);
UmiStatus umi_language_runtime_publish_hover(UmiLanguageRuntimeServiceBridge*b,const char*d,uint32_t line,uint32_t col,const UmiLanguageRuntimeHoverResult*r);
UmiStatus umi_language_runtime_publish_signature(UmiLanguageRuntimeServiceBridge*b,const char*d,uint32_t line,uint32_t col,const UmiLanguageRuntimeSignatureResult*r);
UmiStatus umi_language_runtime_publish_locations(UmiLanguageRuntimeServiceBridge*b,const char*d,const char*symbol_id,int definition,const UmiLanguageRuntimeLocationList*r);
UmiStatus umi_language_runtime_publish_symbols(UmiLanguageRuntimeServiceBridge*b,const char*d,const UmiLanguageRuntimeSymbolList*r);
UmiStatus umi_language_runtime_publish_diagnostics(UmiLanguageRuntimeServiceBridge*b,const char*d,const UmiLanguageRuntimeDiagnosticList*r);
UmiStatus umi_language_runtime_publish_code_actions(UmiLanguageRuntimeServiceBridge*b,const char*d,const UmiLanguageRuntimeCodeActionList*r);
UmiStatus umi_language_runtime_publish_semantic_tokens(UmiLanguageRuntimeServiceBridge*b,const char*d,const UmiLanguageRuntimeSemanticTokens*r);
UmiStatus umi_language_runtime_publish_inlay_hints(UmiLanguageRuntimeServiceBridge*b,const char*d,const UmiLanguageRuntimeInlayHintList*r);
UmiStatus umi_language_runtime_publish_folding_ranges(UmiLanguageRuntimeServiceBridge*b,const char*d,const UmiLanguageRuntimeFoldingRangeList*r);
UmiStatus umi_language_runtime_publish_formatting_available(UmiLanguageRuntimeServiceBridge*b,const char*d,const char*provider,uint32_t tab,int spaces);
UmiStatus umi_language_runtime_publish_rename(UmiLanguageRuntimeServiceBridge*b,const char*d,const char*symbol,const char*old_name,const char*new_name,const UmiLanguageRuntimeWorkspaceEdit*e);
#ifdef __cplusplus
}
#endif
#endif
