/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workspace_search_match.h
 *
 * PURPOSE:
 *   Define stable workspace-search result values shared by search views,
 *   replacement preview, navigation, result export and transaction planning.
 *
 * DESIGN:
 *   Matches store bounded text and capture offsets rather than borrowed source
 *   pointers. This keeps result snapshots valid after the index changes and
 *   makes them safe to pass through command, event and frontend boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKSPACE_SEARCH_MATCH_H
#define UMICOM_EDITOR_WORKSPACE_SEARCH_MATCH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/source_location.h"
#include "umicom/editor/workspace_search_index.h"
#include "umicom/editor/workspace_search_pattern.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_WORKSPACE_SEARCH_MATCH_API_VERSION 1U
#define UMI_EDITOR_WORKSPACE_SEARCH_MATCH_ID_CAPACITY 128U
#define UMI_EDITOR_WORKSPACE_SEARCH_MATCH_TEXT_CAPACITY 512U
#define UMI_EDITOR_WORKSPACE_SEARCH_MATCH_LINE_CAPACITY 512U

typedef struct UmiEditorWorkspaceSearchMatch {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_WORKSPACE_SEARCH_MATCH_ID_CAPACITY];
    UmiEditorSourceLocation location;
    char matched_text[UMI_EDITOR_WORKSPACE_SEARCH_MATCH_TEXT_CAPACITY];
    char line_preview[UMI_EDITOR_WORKSPACE_SEARCH_MATCH_LINE_CAPACITY];
    size_t capture_count;
    UmiEditorWorkspaceSearchCapture
        captures[UMI_EDITOR_WORKSPACE_SEARCH_MAX_CAPTURES];
    uint64_t document_fingerprint;
    uint64_t index_revision;
    uint64_t execution_steps;
    size_t ordinal_in_document;
    int selected;
    int truncated_text;
    int read_only;
    int generated;
} UmiEditorWorkspaceSearchMatch;

UmiStatus umi_editor_workspace_search_match_initialize(
    UmiEditorWorkspaceSearchMatch *match,
    const UmiEditorWorkspaceSearchDocumentView *document,
    const UmiEditorWorkspaceSearchPatternMatch *pattern_match,
    uint64_t index_revision,
    size_t ordinal_in_document);

UmiStatus umi_editor_workspace_search_match_validate(
    const UmiEditorWorkspaceSearchMatch *match);
int umi_editor_workspace_search_match_compare(
    const UmiEditorWorkspaceSearchMatch *left,
    const UmiEditorWorkspaceSearchMatch *right);
UmiStatus umi_editor_workspace_search_match_format(
    const UmiEditorWorkspaceSearchMatch *match,
    char *out_text,
    size_t out_capacity);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_SEARCH_MATCH_H */
