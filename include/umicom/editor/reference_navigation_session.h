/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/reference_navigation_session.h
 *
 * PURPOSE:
 *   Own a professional Find All References experience: provider aggregation,
 *   declaration policy, deterministic file grouping, filtering, collapse and
 *   expansion, keyboard traversal, previews, refresh and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_REFERENCE_NAVIGATION_SESSION_H
#define UMICOM_EDITOR_REFERENCE_NAVIGATION_SESSION_H

#include "umicom/editor/navigation_query_session.h"
#include "umicom/editor/navigation_source_preview.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_REFERENCE_NAVIGATION_SESSION_API_VERSION 1U
#define UMI_EDITOR_REFERENCE_NAVIGATION_FILTER_CAPACITY 256U

typedef enum UmiEditorReferenceNavigationState {
    UMI_EDITOR_REFERENCE_NAVIGATION_CLOSED = 1,
    UMI_EDITOR_REFERENCE_NAVIGATION_LOADING = 2,
    UMI_EDITOR_REFERENCE_NAVIGATION_OPEN = 3,
    UMI_EDITOR_REFERENCE_NAVIGATION_CANCELLED = 4,
    UMI_EDITOR_REFERENCE_NAVIGATION_FAILED = 5
} UmiEditorReferenceNavigationState;

typedef struct UmiEditorReferenceNavigationGroup {
    uint32_t struct_size;
    uint32_t api_version;
    char uri[UMI_EDITOR_SOURCE_URI_CAPACITY];
    char label[UMI_EDITOR_SOURCE_LABEL_CAPACITY];
    size_t result_count;
    size_t visible_result_count;
    int expanded;
} UmiEditorReferenceNavigationGroup;

typedef struct UmiEditorReferenceNavigationEntry {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorNavigationResult result;
    size_t group_index;
    int declaration;
    int visible;
    int selected;
} UmiEditorReferenceNavigationEntry;

typedef struct UmiEditorReferenceNavigationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorReferenceNavigationState state;
    size_t group_count;
    size_t result_count;
    size_t visible_result_count;
    size_t selected_index;
    uint64_t request_id;
    uint64_t revision;
    int include_declaration;
    int has_selection;
    int has_preview;
    int truncated;
    char filter[UMI_EDITOR_REFERENCE_NAVIGATION_FILTER_CAPACITY];
} UmiEditorReferenceNavigationSnapshot;

typedef struct UmiEditorReferenceNavigationSession
    UmiEditorReferenceNavigationSession;

UmiStatus umi_editor_reference_navigation_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorReferenceNavigationSession **out_session);
void umi_editor_reference_navigation_session_destroy(
    UmiEditorReferenceNavigationSession *session);
UmiStatus umi_editor_reference_navigation_session_open(
    UmiEditorReferenceNavigationSession *session,
    const UmiEditorNavigationRequest *request);
UmiStatus umi_editor_reference_navigation_session_refresh(
    UmiEditorReferenceNavigationSession *session);
UmiStatus umi_editor_reference_navigation_session_cancel(
    UmiEditorReferenceNavigationSession *session);
UmiStatus umi_editor_reference_navigation_session_close(
    UmiEditorReferenceNavigationSession *session);
UmiStatus umi_editor_reference_navigation_session_set_filter(
    UmiEditorReferenceNavigationSession *session,
    const char *filter);
UmiStatus umi_editor_reference_navigation_session_set_include_declaration(
    UmiEditorReferenceNavigationSession *session,
    int include_declaration);
UmiStatus umi_editor_reference_navigation_session_set_group_expanded(
    UmiEditorReferenceNavigationSession *session,
    size_t group_index,
    int expanded);
UmiStatus umi_editor_reference_navigation_session_select(
    UmiEditorReferenceNavigationSession *session,
    size_t result_index);
UmiStatus umi_editor_reference_navigation_session_select_next(
    UmiEditorReferenceNavigationSession *session,
    int wrap);
UmiStatus umi_editor_reference_navigation_session_select_previous(
    UmiEditorReferenceNavigationSession *session,
    int wrap);
UmiStatus umi_editor_reference_navigation_session_group_at(
    const UmiEditorReferenceNavigationSession *session,
    size_t group_index,
    UmiEditorReferenceNavigationGroup *out_group);
UmiStatus umi_editor_reference_navigation_session_entry_at(
    const UmiEditorReferenceNavigationSession *session,
    size_t result_index,
    UmiEditorReferenceNavigationEntry *out_entry);
UmiStatus umi_editor_reference_navigation_session_visible_entry_at(
    const UmiEditorReferenceNavigationSession *session,
    size_t visible_index,
    UmiEditorReferenceNavigationEntry *out_entry);
UmiStatus umi_editor_reference_navigation_session_selected(
    const UmiEditorReferenceNavigationSession *session,
    UmiEditorReferenceNavigationEntry *out_entry);
UmiStatus umi_editor_reference_navigation_session_selected_preview(
    const UmiEditorReferenceNavigationSession *session,
    UmiEditorNavigationSourcePreview *out_preview);
UmiStatus umi_editor_reference_navigation_session_snapshot(
    const UmiEditorReferenceNavigationSession *session,
    UmiEditorReferenceNavigationSnapshot *out_snapshot);
UmiEditorNavigationQuerySession *umi_editor_reference_navigation_session_query(
    UmiEditorReferenceNavigationSession *session);
size_t umi_editor_reference_navigation_session_group_count(
    const UmiEditorReferenceNavigationSession *session);
size_t umi_editor_reference_navigation_session_count(
    const UmiEditorReferenceNavigationSession *session);
size_t umi_editor_reference_navigation_session_visible_count(
    const UmiEditorReferenceNavigationSession *session);
uint64_t umi_editor_reference_navigation_session_revision(
    const UmiEditorReferenceNavigationSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_REFERENCE_NAVIGATION_SESSION_H */
