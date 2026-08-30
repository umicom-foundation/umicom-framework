/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/hierarchy_navigation_session.h
 *
 * PURPOSE:
 *   Provide provider-backed, lazily expandable call and type hierarchies with
 *   bounded traversal, cycle detection, filtering, collapse/expand operations,
 *   deterministic selection, evidence and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_HIERARCHY_NAVIGATION_SESSION_H
#define UMICOM_EDITOR_HIERARCHY_NAVIGATION_SESSION_H

#include "umicom/editor/navigation_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_HIERARCHY_NAVIGATION_SESSION_API_VERSION 1U
#define UMI_EDITOR_HIERARCHY_NAVIGATION_FILTER_CAPACITY 256U

typedef enum UmiEditorHierarchyNavigationState {
    UMI_EDITOR_HIERARCHY_NAVIGATION_CLOSED = 1,
    UMI_EDITOR_HIERARCHY_NAVIGATION_LOADING = 2,
    UMI_EDITOR_HIERARCHY_NAVIGATION_OPEN = 3,
    UMI_EDITOR_HIERARCHY_NAVIGATION_CANCELLED = 4,
    UMI_EDITOR_HIERARCHY_NAVIGATION_FAILED = 5
} UmiEditorHierarchyNavigationState;

typedef struct UmiEditorHierarchyNavigationEntry {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorNavigationHierarchyNode node;
    int visible;
    int selected;
} UmiEditorHierarchyNavigationEntry;

typedef struct UmiEditorHierarchyNavigationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorHierarchyNavigationState state;
    UmiEditorNavigationHierarchyKind hierarchy_kind;
    size_t node_count;
    size_t visible_node_count;
    size_t expanded_node_count;
    size_t cycle_count;
    size_t selected_index;
    size_t provider_count;
    uint64_t request_id;
    uint64_t revision;
    int has_selection;
    int truncated;
    int cancelled;
    char filter[UMI_EDITOR_HIERARCHY_NAVIGATION_FILTER_CAPACITY];
} UmiEditorHierarchyNavigationSnapshot;

typedef struct UmiEditorHierarchyNavigationSession
    UmiEditorHierarchyNavigationSession;

UmiStatus umi_editor_hierarchy_navigation_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorHierarchyNavigationSession **out_session);
void umi_editor_hierarchy_navigation_session_destroy(
    UmiEditorHierarchyNavigationSession *session);
UmiStatus umi_editor_hierarchy_navigation_session_open(
    UmiEditorHierarchyNavigationSession *session,
    const UmiEditorNavigationHierarchyRequest *request);
UmiStatus umi_editor_hierarchy_navigation_session_refresh(
    UmiEditorHierarchyNavigationSession *session);
UmiStatus umi_editor_hierarchy_navigation_session_cancel(
    UmiEditorHierarchyNavigationSession *session);
UmiStatus umi_editor_hierarchy_navigation_session_close(
    UmiEditorHierarchyNavigationSession *session);
UmiStatus umi_editor_hierarchy_navigation_session_expand(
    UmiEditorHierarchyNavigationSession *session,
    const char *node_id);
UmiStatus umi_editor_hierarchy_navigation_session_collapse(
    UmiEditorHierarchyNavigationSession *session,
    const char *node_id);
UmiStatus umi_editor_hierarchy_navigation_session_expand_all(
    UmiEditorHierarchyNavigationSession *session);
UmiStatus umi_editor_hierarchy_navigation_session_collapse_all(
    UmiEditorHierarchyNavigationSession *session);
UmiStatus umi_editor_hierarchy_navigation_session_set_filter(
    UmiEditorHierarchyNavigationSession *session,
    const char *filter);
UmiStatus umi_editor_hierarchy_navigation_session_select(
    UmiEditorHierarchyNavigationSession *session,
    const char *node_id);
UmiStatus umi_editor_hierarchy_navigation_session_select_next(
    UmiEditorHierarchyNavigationSession *session,
    int wrap);
UmiStatus umi_editor_hierarchy_navigation_session_select_previous(
    UmiEditorHierarchyNavigationSession *session,
    int wrap);
UmiStatus umi_editor_hierarchy_navigation_session_at(
    const UmiEditorHierarchyNavigationSession *session,
    size_t position,
    UmiEditorHierarchyNavigationEntry *out_entry);
UmiStatus umi_editor_hierarchy_navigation_session_visible_at(
    const UmiEditorHierarchyNavigationSession *session,
    size_t visible_position,
    UmiEditorHierarchyNavigationEntry *out_entry);
UmiStatus umi_editor_hierarchy_navigation_session_selected(
    const UmiEditorHierarchyNavigationSession *session,
    UmiEditorHierarchyNavigationEntry *out_entry);
UmiStatus umi_editor_hierarchy_navigation_session_provider_report_at(
    const UmiEditorHierarchyNavigationSession *session,
    size_t position,
    UmiEditorNavigationProviderReport *out_report);
UmiStatus umi_editor_hierarchy_navigation_session_snapshot(
    const UmiEditorHierarchyNavigationSession *session,
    UmiEditorHierarchyNavigationSnapshot *out_snapshot);
size_t umi_editor_hierarchy_navigation_session_count(
    const UmiEditorHierarchyNavigationSession *session);
size_t umi_editor_hierarchy_navigation_session_visible_count(
    const UmiEditorHierarchyNavigationSession *session);
uint64_t umi_editor_hierarchy_navigation_session_revision(
    const UmiEditorHierarchyNavigationSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_HIERARCHY_NAVIGATION_SESSION_H */
