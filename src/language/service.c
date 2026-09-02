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

/*
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

/*
 * Initialise language service from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_language_service_create(UmiLanguageService **out_owner)
{
    UmiLanguageService *owner; UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_owner = NULL; owner = (UmiLanguageService *)calloc(1U,sizeof(*owner));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    owner->revision = 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_definition_registry_create(&owner->definition);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_provider_registry_create(&owner->provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_document_registry_create(&owner->document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_symbol_registry_create(&owner->symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_completion_registry_create(&owner->completion);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_hover_registry_create(&owner->hover);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_signature_registry_create(&owner->signature);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_diagnostic_registry_create(&owner->diagnostic);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_code_action_registry_create(&owner->code_action);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_formatting_registry_create(&owner->formatting);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_reference_registry_create(&owner->reference);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_rename_registry_create(&owner->rename);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_semantic_token_registry_create(&owner->semantic_token);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_inlay_hint_registry_create(&owner->inlay_hint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_folding_range_registry_create(&owner->folding_range);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_compilation_database_create(&owner->compilation_database);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_server_profile_registry_create(&owner->server_profiles);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_request_ledger_create(&owner->request_ledger);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_navigation_history_create(&owner->navigation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_language_register_builtin_definitions(owner->definition);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) { umi_language_service_destroy(owner); return status; }
    *out_owner = owner; return UMI_STATUS_OK;
}

/*
 * Release or reset state held by language service so the same storage can be reused
 * safely.
 */
void umi_language_service_destroy(UmiLanguageService *owner)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the language service snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_service_snapshot(const UmiLanguageService *owner, UmiLanguageServiceSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
        /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the language service definition operation used by this module and its client
 * applications.
 */
UmiLanguageDefinitionRegistry *umi_language_service_definition(UmiLanguageService *owner) { return owner != NULL ? owner->definition : NULL; }
/*
 * Provide the language service provider operation used by this module and its client
 * applications.
 */
UmiLanguageProviderRegistry *umi_language_service_provider(UmiLanguageService *owner) { return owner != NULL ? owner->provider : NULL; }
/*
 * Provide the language service document operation used by this module and its client
 * applications.
 */
UmiLanguageDocumentRegistry *umi_language_service_document(UmiLanguageService *owner) { return owner != NULL ? owner->document : NULL; }
/*
 * Provide the language service symbol operation used by this module and its client
 * applications.
 */
UmiLanguageSymbolRegistry *umi_language_service_symbol(UmiLanguageService *owner) { return owner != NULL ? owner->symbol : NULL; }
/*
 * Provide the language service completion operation used by this module and its client
 * applications.
 */
UmiLanguageCompletionRegistry *umi_language_service_completion(UmiLanguageService *owner) { return owner != NULL ? owner->completion : NULL; }
/*
 * Provide the language service hover operation used by this module and its client
 * applications.
 */
UmiLanguageHoverRegistry *umi_language_service_hover(UmiLanguageService *owner) { return owner != NULL ? owner->hover : NULL; }
/*
 * Provide the language service signature operation used by this module and its client
 * applications.
 */
UmiLanguageSignatureRegistry *umi_language_service_signature(UmiLanguageService *owner) { return owner != NULL ? owner->signature : NULL; }
/*
 * Provide the language service diagnostic operation used by this module and its client
 * applications.
 */
UmiLanguageDiagnosticRegistry *umi_language_service_diagnostic(UmiLanguageService *owner) { return owner != NULL ? owner->diagnostic : NULL; }
/*
 * Provide the language service code action operation used by this module and its client
 * applications.
 */
UmiLanguageCodeActionRegistry *umi_language_service_code_action(UmiLanguageService *owner) { return owner != NULL ? owner->code_action : NULL; }
/*
 * Provide the language service formatting operation used by this module and its client
 * applications.
 */
UmiLanguageFormattingRegistry *umi_language_service_formatting(UmiLanguageService *owner) { return owner != NULL ? owner->formatting : NULL; }
/*
 * Provide the language service reference operation used by this module and its client
 * applications.
 */
UmiLanguageReferenceRegistry *umi_language_service_reference(UmiLanguageService *owner) { return owner != NULL ? owner->reference : NULL; }
/*
 * Provide the language service rename operation used by this module and its client
 * applications.
 */
UmiLanguageRenameRegistry *umi_language_service_rename(UmiLanguageService *owner) { return owner != NULL ? owner->rename : NULL; }
/*
 * Provide the language service semantic token operation used by this module and its client
 * applications.
 */
UmiLanguageSemanticTokenRegistry *umi_language_service_semantic_token(UmiLanguageService *owner) { return owner != NULL ? owner->semantic_token : NULL; }
/*
 * Provide the language service inlay hint operation used by this module and its client
 * applications.
 */
UmiLanguageInlayHintRegistry *umi_language_service_inlay_hint(UmiLanguageService *owner) { return owner != NULL ? owner->inlay_hint : NULL; }
/*
 * Provide the language service folding range operation used by this module and its client
 * applications.
 */
UmiLanguageFoldingRangeRegistry *umi_language_service_folding_range(UmiLanguageService *owner) { return owner != NULL ? owner->folding_range : NULL; }
/*
 * Provide the language service compilation database operation used by this module and its
 * client applications.
 */
UmiCompilationDatabase *umi_language_service_compilation_database(UmiLanguageService *owner) { return owner != NULL ? owner->compilation_database : NULL; }
/*
 * Provide the language service server profiles operation used by this module and its
 * client applications.
 */
UmiLanguageServerProfileRegistry *umi_language_service_server_profiles(UmiLanguageService *owner) { return owner != NULL ? owner->server_profiles : NULL; }
/*
 * Provide the language service request ledger operation used by this module and its client
 * applications.
 */
UmiLanguageRequestLedger *umi_language_service_request_ledger(UmiLanguageService *owner) { return owner != NULL ? owner->request_ledger : NULL; }
/*
 * Provide the language service navigation operation used by this module and its client
 * applications.
 */
UmiLanguageNavigationHistory *umi_language_service_navigation(UmiLanguageService *owner) { return owner != NULL ? owner->navigation : NULL; }
