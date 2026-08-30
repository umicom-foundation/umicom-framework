/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/service.c
 *
 * PURPOSE:
 *   Implement the provider-neutral language-intelligence service aggregating documents, symbols, diagnostics, completion, navigation and refactoring data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/language/service.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/language/builtins.h"

struct UmiLanguageService {
    UmiLanguageDefinitionRegistry *definition;
    UmiLanguageProviderRegistry *provider;
    UmiLanguageDocumentRegistry *document;
    UmiLanguageSymbolRegistry *symbol;
    UmiLanguageCompletionRegistry *completion;
    UmiLanguageHoverRegistry *hover;
    UmiLanguageSignatureRegistry *signature;
    UmiLanguageDiagnosticRegistry *diagnostic;
    UmiLanguageCodeActionRegistry *code_action;
    UmiLanguageFormattingRegistry *formatting;
    UmiLanguageReferenceRegistry *reference;
    UmiLanguageRenameRegistry *rename;
    UmiLanguageSemanticTokenRegistry *semantic_token;
    UmiLanguageInlayHintRegistry *inlay_hint;
    UmiLanguageFoldingRangeRegistry *folding_range;
    UmiCompilationDatabase *compilation_database;
    UmiLanguageServerProfileRegistry *server_profiles;
    UmiLanguageRequestLedger *request_ledger;
    UmiLanguageNavigationHistory *navigation;
    uint64_t revision;
};

UmiStatus umi_language_service_create(UmiLanguageService **out_owner)
{
    UmiLanguageService *owner; UmiStatus status = UMI_STATUS_OK;
    if (out_owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_owner = NULL; owner = (UmiLanguageService *)calloc(1U,sizeof(*owner));
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    owner->revision = 1U;
    if (status == UMI_STATUS_OK) status = umi_language_definition_registry_create(&owner->definition);
    if (status == UMI_STATUS_OK) status = umi_language_provider_registry_create(&owner->provider);
    if (status == UMI_STATUS_OK) status = umi_language_document_registry_create(&owner->document);
    if (status == UMI_STATUS_OK) status = umi_language_symbol_registry_create(&owner->symbol);
    if (status == UMI_STATUS_OK) status = umi_language_completion_registry_create(&owner->completion);
    if (status == UMI_STATUS_OK) status = umi_language_hover_registry_create(&owner->hover);
    if (status == UMI_STATUS_OK) status = umi_language_signature_registry_create(&owner->signature);
    if (status == UMI_STATUS_OK) status = umi_language_diagnostic_registry_create(&owner->diagnostic);
    if (status == UMI_STATUS_OK) status = umi_language_code_action_registry_create(&owner->code_action);
    if (status == UMI_STATUS_OK) status = umi_language_formatting_registry_create(&owner->formatting);
    if (status == UMI_STATUS_OK) status = umi_language_reference_registry_create(&owner->reference);
    if (status == UMI_STATUS_OK) status = umi_language_rename_registry_create(&owner->rename);
    if (status == UMI_STATUS_OK) status = umi_language_semantic_token_registry_create(&owner->semantic_token);
    if (status == UMI_STATUS_OK) status = umi_language_inlay_hint_registry_create(&owner->inlay_hint);
    if (status == UMI_STATUS_OK) status = umi_language_folding_range_registry_create(&owner->folding_range);
    if (status == UMI_STATUS_OK) status = umi_compilation_database_create(&owner->compilation_database);
    if (status == UMI_STATUS_OK) status = umi_language_server_profile_registry_create(&owner->server_profiles);
    if (status == UMI_STATUS_OK) status = umi_language_request_ledger_create(&owner->request_ledger);
    if (status == UMI_STATUS_OK) status = umi_language_navigation_history_create(&owner->navigation);
    if (status == UMI_STATUS_OK) status = umi_language_register_builtin_definitions(owner->definition);
    if (status != UMI_STATUS_OK) { umi_language_service_destroy(owner); return status; }
    *out_owner = owner; return UMI_STATUS_OK;
}

void umi_language_service_destroy(UmiLanguageService *owner)
{
    if (owner == NULL) return;
    umi_language_navigation_history_destroy(owner->navigation);
    umi_language_request_ledger_destroy(owner->request_ledger);
    umi_language_server_profile_registry_destroy(owner->server_profiles);
    umi_compilation_database_destroy(owner->compilation_database);
    umi_language_folding_range_registry_destroy(owner->folding_range);
    umi_language_inlay_hint_registry_destroy(owner->inlay_hint);
    umi_language_semantic_token_registry_destroy(owner->semantic_token);
    umi_language_rename_registry_destroy(owner->rename);
    umi_language_reference_registry_destroy(owner->reference);
    umi_language_formatting_registry_destroy(owner->formatting);
    umi_language_code_action_registry_destroy(owner->code_action);
    umi_language_diagnostic_registry_destroy(owner->diagnostic);
    umi_language_signature_registry_destroy(owner->signature);
    umi_language_hover_registry_destroy(owner->hover);
    umi_language_completion_registry_destroy(owner->completion);
    umi_language_symbol_registry_destroy(owner->symbol);
    umi_language_document_registry_destroy(owner->document);
    umi_language_provider_registry_destroy(owner->provider);
    umi_language_definition_registry_destroy(owner->definition);
    free(owner);
}

UmiStatus umi_language_service_snapshot(const UmiLanguageService *owner, UmiLanguageServiceSnapshot *out_snapshot)
{
    if (owner == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot,0,sizeof(*out_snapshot));
    out_snapshot->struct_size=(uint32_t)sizeof(*out_snapshot); out_snapshot->api_version=1U;
    out_snapshot->revision=owner->revision;
    out_snapshot->definition_count = umi_language_definition_registry_count(owner->definition);
    out_snapshot->provider_count = umi_language_provider_registry_count(owner->provider);
    out_snapshot->document_count = umi_language_document_registry_count(owner->document);
    out_snapshot->symbol_count = umi_language_symbol_registry_count(owner->symbol);
    out_snapshot->completion_count = umi_language_completion_registry_count(owner->completion);
    out_snapshot->hover_count = umi_language_hover_registry_count(owner->hover);
    out_snapshot->signature_count = umi_language_signature_registry_count(owner->signature);
    out_snapshot->diagnostic_count = umi_language_diagnostic_registry_count(owner->diagnostic);
    out_snapshot->code_action_count = umi_language_code_action_registry_count(owner->code_action);
    out_snapshot->formatting_count = umi_language_formatting_registry_count(owner->formatting);
    out_snapshot->reference_count = umi_language_reference_registry_count(owner->reference);
    out_snapshot->rename_count = umi_language_rename_registry_count(owner->rename);
    out_snapshot->semantic_token_count = umi_language_semantic_token_registry_count(owner->semantic_token);
    out_snapshot->inlay_hint_count = umi_language_inlay_hint_registry_count(owner->inlay_hint);
    out_snapshot->folding_range_count = umi_language_folding_range_registry_count(owner->folding_range);
    {
        UmiCompilationDatabaseSnapshot compilation_database;
        if (umi_compilation_database_snapshot(owner->compilation_database,
                                              &compilation_database) ==
            UMI_STATUS_OK) {
            out_snapshot->compilation_command_count =
                compilation_database.command_count;
        }
    }
    out_snapshot->server_profile_count = umi_language_server_profile_registry_count(owner->server_profiles);
    out_snapshot->pending_request_count = umi_language_request_ledger_pending(owner->request_ledger);
    out_snapshot->navigation_location_count = umi_language_navigation_history_count(owner->navigation);
    out_snapshot->item_count = out_snapshot->definition_count + out_snapshot->provider_count + out_snapshot->document_count + out_snapshot->symbol_count + out_snapshot->completion_count + out_snapshot->hover_count + out_snapshot->signature_count + out_snapshot->diagnostic_count + out_snapshot->code_action_count + out_snapshot->formatting_count + out_snapshot->reference_count + out_snapshot->rename_count + out_snapshot->semantic_token_count + out_snapshot->inlay_hint_count + out_snapshot->folding_range_count + out_snapshot->compilation_command_count + out_snapshot->server_profile_count + out_snapshot->navigation_location_count;
    return UMI_STATUS_OK;
}

UmiLanguageDefinitionRegistry *umi_language_service_definition(UmiLanguageService *owner) { return owner != NULL ? owner->definition : NULL; }
UmiLanguageProviderRegistry *umi_language_service_provider(UmiLanguageService *owner) { return owner != NULL ? owner->provider : NULL; }
UmiLanguageDocumentRegistry *umi_language_service_document(UmiLanguageService *owner) { return owner != NULL ? owner->document : NULL; }
UmiLanguageSymbolRegistry *umi_language_service_symbol(UmiLanguageService *owner) { return owner != NULL ? owner->symbol : NULL; }
UmiLanguageCompletionRegistry *umi_language_service_completion(UmiLanguageService *owner) { return owner != NULL ? owner->completion : NULL; }
UmiLanguageHoverRegistry *umi_language_service_hover(UmiLanguageService *owner) { return owner != NULL ? owner->hover : NULL; }
UmiLanguageSignatureRegistry *umi_language_service_signature(UmiLanguageService *owner) { return owner != NULL ? owner->signature : NULL; }
UmiLanguageDiagnosticRegistry *umi_language_service_diagnostic(UmiLanguageService *owner) { return owner != NULL ? owner->diagnostic : NULL; }
UmiLanguageCodeActionRegistry *umi_language_service_code_action(UmiLanguageService *owner) { return owner != NULL ? owner->code_action : NULL; }
UmiLanguageFormattingRegistry *umi_language_service_formatting(UmiLanguageService *owner) { return owner != NULL ? owner->formatting : NULL; }
UmiLanguageReferenceRegistry *umi_language_service_reference(UmiLanguageService *owner) { return owner != NULL ? owner->reference : NULL; }
UmiLanguageRenameRegistry *umi_language_service_rename(UmiLanguageService *owner) { return owner != NULL ? owner->rename : NULL; }
UmiLanguageSemanticTokenRegistry *umi_language_service_semantic_token(UmiLanguageService *owner) { return owner != NULL ? owner->semantic_token : NULL; }
UmiLanguageInlayHintRegistry *umi_language_service_inlay_hint(UmiLanguageService *owner) { return owner != NULL ? owner->inlay_hint : NULL; }
UmiLanguageFoldingRangeRegistry *umi_language_service_folding_range(UmiLanguageService *owner) { return owner != NULL ? owner->folding_range : NULL; }
UmiCompilationDatabase *umi_language_service_compilation_database(UmiLanguageService *owner) { return owner != NULL ? owner->compilation_database : NULL; }
UmiLanguageServerProfileRegistry *umi_language_service_server_profiles(UmiLanguageService *owner) { return owner != NULL ? owner->server_profiles : NULL; }
UmiLanguageRequestLedger *umi_language_service_request_ledger(UmiLanguageService *owner) { return owner != NULL ? owner->request_ledger : NULL; }
UmiLanguageNavigationHistory *umi_language_service_navigation(UmiLanguageService *owner) { return owner != NULL ? owner->navigation : NULL; }
