/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/code_action_query.h
 *
 * PURPOSE:
 *   Aggregate, filter and rank provider code actions for a document context
 *   while preserving the existing canonical UmiEditorCodeActionSnapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_CODE_ACTION_QUERY_H
#define UMICOM_EDITOR_CODE_ACTION_QUERY_H

#include "umicom/editor/code_action.h"
#include "umicom/editor/code_action_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_CODE_ACTION_QUERY_API_VERSION 1U
#define UMI_EDITOR_CODE_ACTION_QUERY_ID_CAPACITY 128U
#define UMI_EDITOR_CODE_ACTION_QUERY_URI_CAPACITY 1024U
#define UMI_EDITOR_CODE_ACTION_DIAGNOSTIC_ID_CAPACITY 128U
#define UMI_EDITOR_CODE_ACTION_DISABLED_REASON_CAPACITY 256U

/**
 * List the named editor code action trigger values accepted by this public contract.
 */
typedef enum UmiEditorCodeActionTrigger {
    UMI_EDITOR_CODE_ACTION_TRIGGER_AUTOMATIC = 1,
    UMI_EDITOR_CODE_ACTION_TRIGGER_INVOKED = 2,
    UMI_EDITOR_CODE_ACTION_TRIGGER_DIAGNOSTIC = 3,
    UMI_EDITOR_CODE_ACTION_TRIGGER_SAVE = 4
} UmiEditorCodeActionTrigger;

/**
 * Represent the editor code action query request data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCodeActionQueryRequest {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_CODE_ACTION_QUERY_ID_CAPACITY];
    char document_id[128];
    char document_uri[UMI_EDITOR_CODE_ACTION_QUERY_URI_CAPACITY];
    char language_id[UMI_EDITOR_CODE_ACTION_PROVIDER_LANGUAGE_CAPACITY];
    char diagnostic_id[UMI_EDITOR_CODE_ACTION_DIAGNOSTIC_ID_CAPACITY];
    UmiEditorCodeActionKindMask requested_kind_mask;
    UmiEditorCodeActionTrigger trigger;
    uint64_t start_byte_offset;
    uint64_t end_byte_offset;
    uint64_t document_revision;
    int include_disabled;
} UmiEditorCodeActionQueryRequest;

/**
 * Represent the editor ranked code action data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorRankedCodeAction {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorCodeActionSnapshot action;
    char provider_id[UMI_EDITOR_CODE_ACTION_PROVIDER_ID_CAPACITY];
    char diagnostic_id[UMI_EDITOR_CODE_ACTION_DIAGNOSTIC_ID_CAPACITY];
    char disabled_reason[UMI_EDITOR_CODE_ACTION_DISABLED_REASON_CAPACITY];
    UmiEditorCodeActionKindMask kind_mask;
    int32_t provider_priority;
    int32_t relevance_score;
    int requires_resolution;
    int safe;
    int supports_preview;
} UmiEditorRankedCodeAction;

/**
 * Represent the editor code action query snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCodeActionQuerySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorCodeActionQueryRequest request;
    size_t result_count;
    size_t enabled_count;
    size_t preferred_count;
    size_t unresolved_count;
    size_t safe_count;
    uint64_t revision;
    int finalized;
} UmiEditorCodeActionQuerySnapshot;

/**
 * Represent the editor code action query data shared with callers of this public contract.
 */
typedef struct UmiEditorCodeActionQuery UmiEditorCodeActionQuery;

/**
 * Initialise editor code action query from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_code_action_query_create(
    UmiEditorCodeActionQuery **out_query);
/**
 * Release or reset state held by editor code action query so the same storage can be
 * reused safely.
 */
void umi_editor_code_action_query_destroy(UmiEditorCodeActionQuery *query);
/**
 * Provide the editor code action query begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_code_action_query_begin(
    UmiEditorCodeActionQuery *query,
    const UmiEditorCodeActionQueryRequest *request);
/**
 * Add editor code action query only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_code_action_query_add(
    UmiEditorCodeActionQuery *query,
    const UmiEditorCodeActionProviderRegistry *providers,
    const UmiEditorRankedCodeAction *candidate);
/**
 * Provide the editor code action query finalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_code_action_query_finalize(UmiEditorCodeActionQuery *query);
/**
 * Find editor code action query while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_code_action_query_at(
    const UmiEditorCodeActionQuery *query,
    size_t index,
    UmiEditorRankedCodeAction *out_action);
/**
 * Find editor code action query while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_code_action_query_find(
    const UmiEditorCodeActionQuery *query,
    const char *action_id,
    UmiEditorRankedCodeAction *out_action);
/**
 * Provide the editor code action query snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_code_action_query_snapshot(
    const UmiEditorCodeActionQuery *query,
    UmiEditorCodeActionQuerySnapshot *out_snapshot);
/**
 * Return the number of records represented by editor code action query without changing
 * their state.
 */
size_t umi_editor_code_action_query_count(
    const UmiEditorCodeActionQuery *query);
/**
 * Provide the editor code action query revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_code_action_query_revision(
    const UmiEditorCodeActionQuery *query);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_CODE_ACTION_QUERY_H */
