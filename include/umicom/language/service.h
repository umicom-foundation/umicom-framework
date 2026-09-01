/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/service.h
 *
 * PURPOSE:
 *   Define the provider-neutral language-intelligence service aggregating documents, symbols, diagnostics, completion, navigation and refactoring data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef FRAMEWORK_INCLUDE_UMICOM_LANGUAGE_SERVICE_H
#define FRAMEWORK_INCLUDE_UMICOM_LANGUAGE_SERVICE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/language/definition.h"
#include "umicom/language/provider.h"
#include "umicom/language/document.h"
#include "umicom/language/symbol.h"
#include "umicom/language/completion.h"
#include "umicom/language/hover.h"
#include "umicom/language/signature.h"
#include "umicom/language/diagnostic.h"
#include "umicom/language/code_action.h"
#include "umicom/language/formatting.h"
#include "umicom/language/reference.h"
#include "umicom/language/rename.h"
#include "umicom/language/semantic_token.h"
#include "umicom/language/inlay_hint.h"
#include "umicom/language/folding_range.h"
#include "umicom/toolchain/compilation_database.h"
#include "umicom/language/server_profile.h"
#include "umicom/language/request_ledger.h"
#include "umicom/language/navigation_history.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiLanguageService UmiLanguageService;
typedef struct UmiLanguageServiceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t revision;
    size_t item_count;
    size_t definition_count;
    size_t provider_count;
    size_t document_count;
    size_t symbol_count;
    size_t completion_count;
    size_t hover_count;
    size_t signature_count;
    size_t diagnostic_count;
    size_t code_action_count;
    size_t formatting_count;
    size_t reference_count;
    size_t rename_count;
    size_t semantic_token_count;
    size_t inlay_hint_count;
    size_t folding_range_count;
    size_t compilation_command_count;
    size_t server_profile_count;
    size_t pending_request_count;
    size_t navigation_location_count;
} UmiLanguageServiceSnapshot;

UmiStatus umi_language_service_create(UmiLanguageService **out_owner);
void umi_language_service_destroy(UmiLanguageService *owner);
UmiStatus umi_language_service_snapshot(const UmiLanguageService *owner, UmiLanguageServiceSnapshot *out_snapshot);
UmiLanguageDefinitionRegistry *umi_language_service_definition(UmiLanguageService *owner);
UmiLanguageProviderRegistry *umi_language_service_provider(UmiLanguageService *owner);
UmiLanguageDocumentRegistry *umi_language_service_document(UmiLanguageService *owner);
UmiLanguageSymbolRegistry *umi_language_service_symbol(UmiLanguageService *owner);
UmiLanguageCompletionRegistry *umi_language_service_completion(UmiLanguageService *owner);
UmiLanguageHoverRegistry *umi_language_service_hover(UmiLanguageService *owner);
UmiLanguageSignatureRegistry *umi_language_service_signature(UmiLanguageService *owner);
UmiLanguageDiagnosticRegistry *umi_language_service_diagnostic(UmiLanguageService *owner);
UmiLanguageCodeActionRegistry *umi_language_service_code_action(UmiLanguageService *owner);
UmiLanguageFormattingRegistry *umi_language_service_formatting(UmiLanguageService *owner);
UmiLanguageReferenceRegistry *umi_language_service_reference(UmiLanguageService *owner);
UmiLanguageRenameRegistry *umi_language_service_rename(UmiLanguageService *owner);
UmiLanguageSemanticTokenRegistry *umi_language_service_semantic_token(UmiLanguageService *owner);
UmiLanguageInlayHintRegistry *umi_language_service_inlay_hint(UmiLanguageService *owner);
UmiLanguageFoldingRangeRegistry *umi_language_service_folding_range(UmiLanguageService *owner);
UmiCompilationDatabase *umi_language_service_compilation_database(UmiLanguageService *owner);
UmiLanguageServerProfileRegistry *umi_language_service_server_profiles(UmiLanguageService *owner);
UmiLanguageRequestLedger *umi_language_service_request_ledger(UmiLanguageService *owner);
UmiLanguageNavigationHistory *umi_language_service_navigation(UmiLanguageService *owner);

#ifdef __cplusplus
}
#endif
#endif
