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
/**
 * Represent the language runtime service bridge data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeServiceBridge{UmiLanguageService*language;uint64_t revision;}UmiLanguageRuntimeServiceBridge;
/**
 * Initialise language runtime service bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_runtime_service_bridge_init(UmiLanguageRuntimeServiceBridge*b,UmiLanguageService*l);
/**
 * Provide the language runtime publish completion operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_completion(UmiLanguageRuntimeServiceBridge*b,const char*d,uint32_t line,uint32_t col,const UmiLanguageRuntimeCompletionResult*r);
/**
 * Provide the language runtime publish hover operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_publish_hover(UmiLanguageRuntimeServiceBridge*b,const char*d,uint32_t line,uint32_t col,const UmiLanguageRuntimeHoverResult*r);
/**
 * Provide the language runtime publish signature operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_signature(UmiLanguageRuntimeServiceBridge*b,const char*d,uint32_t line,uint32_t col,const UmiLanguageRuntimeSignatureResult*r);
/**
 * Provide the language runtime publish locations operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_locations(UmiLanguageRuntimeServiceBridge*b,const char*d,const char*symbol_id,int definition,const UmiLanguageRuntimeLocationList*r);
/**
 * Provide the language runtime publish symbols operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_symbols(UmiLanguageRuntimeServiceBridge*b,const char*d,const UmiLanguageRuntimeSymbolList*r);
/**
 * Provide the language runtime publish diagnostics operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_diagnostics(UmiLanguageRuntimeServiceBridge*b,const char*d,const UmiLanguageRuntimeDiagnosticList*r);
/**
 * Provide the language runtime publish code actions operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_code_actions(UmiLanguageRuntimeServiceBridge*b,const char*d,const UmiLanguageRuntimeCodeActionList*r);
/**
 * Provide the language runtime publish semantic tokens operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_publish_semantic_tokens(UmiLanguageRuntimeServiceBridge*b,const char*d,const UmiLanguageRuntimeSemanticTokens*r);
/**
 * Provide the language runtime publish inlay hints operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_publish_inlay_hints(UmiLanguageRuntimeServiceBridge*b,const char*d,const UmiLanguageRuntimeInlayHintList*r);
/**
 * Provide the language runtime publish folding ranges operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_publish_folding_ranges(UmiLanguageRuntimeServiceBridge*b,const char*d,const UmiLanguageRuntimeFoldingRangeList*r);
/**
 * Provide the language runtime publish formatting available operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_publish_formatting_available(UmiLanguageRuntimeServiceBridge*b,const char*d,const char*provider,uint32_t tab,int spaces);
/**
 * Provide the language runtime publish rename operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_publish_rename(UmiLanguageRuntimeServiceBridge*b,const char*d,const char*symbol,const char*old_name,const char*new_name,const UmiLanguageRuntimeWorkspaceEdit*e);
#ifdef __cplusplus
}
#endif
#endif
