/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/code_action_query.h
 *
 * PURPOSE:
 *   Aggregate, filter and rank provider code actions for a document context
 *   while preserving the existing canonical UmiEditorCodeActionSnapshot.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef enum UmiEditorCodeActionTrigger {
    UMI_EDITOR_CODE_ACTION_TRIGGER_AUTOMATIC = 1,
    UMI_EDITOR_CODE_ACTION_TRIGGER_INVOKED = 2,
    UMI_EDITOR_CODE_ACTION_TRIGGER_DIAGNOSTIC = 3,
    UMI_EDITOR_CODE_ACTION_TRIGGER_SAVE = 4
} UmiEditorCodeActionTrigger;

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

typedef struct UmiEditorCodeActionQuery UmiEditorCodeActionQuery;

UmiStatus umi_editor_code_action_query_create(
    UmiEditorCodeActionQuery **out_query);
void umi_editor_code_action_query_destroy(UmiEditorCodeActionQuery *query);
UmiStatus umi_editor_code_action_query_begin(
    UmiEditorCodeActionQuery *query,
    const UmiEditorCodeActionQueryRequest *request);
UmiStatus umi_editor_code_action_query_add(
    UmiEditorCodeActionQuery *query,
    const UmiEditorCodeActionProviderRegistry *providers,
    const UmiEditorRankedCodeAction *candidate);
UmiStatus umi_editor_code_action_query_finalize(UmiEditorCodeActionQuery *query);
UmiStatus umi_editor_code_action_query_at(
    const UmiEditorCodeActionQuery *query,
    size_t index,
    UmiEditorRankedCodeAction *out_action);
UmiStatus umi_editor_code_action_query_find(
    const UmiEditorCodeActionQuery *query,
    const char *action_id,
    UmiEditorRankedCodeAction *out_action);
UmiStatus umi_editor_code_action_query_snapshot(
    const UmiEditorCodeActionQuery *query,
    UmiEditorCodeActionQuerySnapshot *out_snapshot);
size_t umi_editor_code_action_query_count(
    const UmiEditorCodeActionQuery *query);
uint64_t umi_editor_code_action_query_revision(
    const UmiEditorCodeActionQuery *query);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_CODE_ACTION_QUERY_H */
