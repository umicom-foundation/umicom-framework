/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workspace_search_preview.h
 *
 * PURPOSE:
 *   Define replacement-template expansion and conflict-aware, per-match preview
 *   values before any workspace document is changed.
 *
 * SUPPORTED TEMPLATE TOKENS:
 *   $0 or $&  complete match
 *   $1..$8    captured groups
 *   $$        literal dollar sign
 *   \n \r \t  escaped control characters when enabled
 *   \\        literal backslash when escape interpretation is enabled
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKSPACE_SEARCH_PREVIEW_H
#define UMICOM_EDITOR_WORKSPACE_SEARCH_PREVIEW_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/workspace_search_index.h"
#include "umicom/editor/workspace_search_match.h"
#include "umicom/editor/workspace_search_query.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_WORKSPACE_SEARCH_PREVIEW_API_VERSION 1U
#define UMI_EDITOR_WORKSPACE_REPLACEMENT_TEMPLATE_CAPACITY 512U
#define UMI_EDITOR_WORKSPACE_REPLACEMENT_TEXT_CAPACITY 512U
#define UMI_EDITOR_WORKSPACE_REPLACEMENT_CONTEXT_CAPACITY 1024U
#define UMI_EDITOR_WORKSPACE_REPLACEMENT_REASON_CAPACITY 256U

/**
 * Represent the editor workspace replacement request data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorWorkspaceReplacementRequest {
    uint32_t struct_size;
    uint32_t api_version;
    char replacement_template[UMI_EDITOR_WORKSPACE_REPLACEMENT_TEMPLATE_CAPACITY];
    int interpret_escapes;
    int preserve_case;
} UmiEditorWorkspaceReplacementRequest;

/**
 * List the named editor workspace replacement preview state values accepted by this public
 * contract.
 */
typedef enum UmiEditorWorkspaceReplacementPreviewState {
    UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_READY = 1,
    UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_READ_ONLY = 2,
    UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_STALE = 3,
    UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_TRUNCATED_MATCH = 4,
    UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_INVALID_TEMPLATE = 5,
    UMI_EDITOR_WORKSPACE_REPLACEMENT_PREVIEW_REPLACEMENT_TOO_LARGE = 6
} UmiEditorWorkspaceReplacementPreviewState;

/**
 * Represent the editor workspace replacement preview item data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorWorkspaceReplacementPreviewItem {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorWorkspaceSearchMatch match;
    char replacement_text[UMI_EDITOR_WORKSPACE_REPLACEMENT_TEXT_CAPACITY];
    char original_context[UMI_EDITOR_WORKSPACE_REPLACEMENT_CONTEXT_CAPACITY];
    char replacement_context[UMI_EDITOR_WORKSPACE_REPLACEMENT_CONTEXT_CAPACITY];
    char reason[UMI_EDITOR_WORKSPACE_REPLACEMENT_REASON_CAPACITY];
    UmiEditorWorkspaceReplacementPreviewState state;
    uint64_t source_query_revision;
    uint64_t source_index_revision;
    int selected;
    int applicable;
} UmiEditorWorkspaceReplacementPreviewItem;

/**
 * Represent the editor workspace replacement preview snapshot data shared with callers of
 * this public contract.
 */
typedef struct UmiEditorWorkspaceReplacementPreviewSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t item_count;
    size_t selected_item_count;
    size_t applicable_item_count;
    size_t conflict_item_count;
    size_t document_count;
    uint64_t source_query_revision;
    uint64_t source_index_revision;
    uint64_t revision;
    int ready;
} UmiEditorWorkspaceReplacementPreviewSnapshot;

/**
 * Represent the editor workspace replacement preview data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorWorkspaceReplacementPreview
    UmiEditorWorkspaceReplacementPreview;

/**
 * Initialise editor workspace replacement request from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_workspace_replacement_request_init(
    UmiEditorWorkspaceReplacementRequest *request,
    const char *replacement_template);

/**
 * Initialise editor workspace replacement preview from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_workspace_replacement_preview_create(
    UmiEditorWorkspaceReplacementPreview **out_preview);
/**
 * Release or reset state held by editor workspace replacement preview so the same storage
 * can be reused safely.
 */
void umi_editor_workspace_replacement_preview_destroy(
    UmiEditorWorkspaceReplacementPreview *preview);
/**
 * Release or reset state held by editor workspace replacement preview so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_workspace_replacement_preview_clear(
    UmiEditorWorkspaceReplacementPreview *preview);

/**
 * Provide the editor workspace replacement preview build operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_workspace_replacement_preview_build(
    UmiEditorWorkspaceReplacementPreview *preview,
    const UmiEditorWorkspaceSearchIndex *index,
    const UmiEditorWorkspaceSearchQuery *query,
    const UmiEditorWorkspaceReplacementRequest *request);
/**
 * Find editor workspace replacement preview while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_workspace_replacement_preview_at(
    const UmiEditorWorkspaceReplacementPreview *preview,
    size_t position,
    UmiEditorWorkspaceReplacementPreviewItem *out_item);
/**
 * Find editor workspace replacement preview while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_workspace_replacement_preview_find(
    const UmiEditorWorkspaceReplacementPreview *preview,
    const char *match_id,
    UmiEditorWorkspaceReplacementPreviewItem *out_item);
/**
 * Provide the editor workspace replacement preview select operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_replacement_preview_select(
    UmiEditorWorkspaceReplacementPreview *preview,
    const char *match_id,
    int selected);
/**
 * Provide the editor workspace replacement preview select all operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_workspace_replacement_preview_select_all(
    UmiEditorWorkspaceReplacementPreview *preview,
    int selected);
/**
 * Provide the editor workspace replacement preview snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_replacement_preview_snapshot(
    const UmiEditorWorkspaceReplacementPreview *preview,
    UmiEditorWorkspaceReplacementPreviewSnapshot *out_snapshot);

/**
 * Return the number of records represented by editor workspace replacement preview without
 * changing their state.
 */
size_t umi_editor_workspace_replacement_preview_count(
    const UmiEditorWorkspaceReplacementPreview *preview);
/**
 * Provide the editor workspace replacement preview revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_workspace_replacement_preview_revision(
    const UmiEditorWorkspaceReplacementPreview *preview);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_SEARCH_PREVIEW_H */
