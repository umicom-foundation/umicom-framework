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

/**
 * List the named editor hierarchy navigation state values accepted by this public
 * contract.
 */
typedef enum UmiEditorHierarchyNavigationState {
    UMI_EDITOR_HIERARCHY_NAVIGATION_CLOSED = 1,
    UMI_EDITOR_HIERARCHY_NAVIGATION_LOADING = 2,
    UMI_EDITOR_HIERARCHY_NAVIGATION_OPEN = 3,
    UMI_EDITOR_HIERARCHY_NAVIGATION_CANCELLED = 4,
    UMI_EDITOR_HIERARCHY_NAVIGATION_FAILED = 5
} UmiEditorHierarchyNavigationState;

/**
 * Represent the editor hierarchy navigation entry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorHierarchyNavigationEntry {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorNavigationHierarchyNode node;
    int visible;
    int selected;
} UmiEditorHierarchyNavigationEntry;

/**
 * Represent the editor hierarchy navigation snapshot data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the editor hierarchy navigation session data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorHierarchyNavigationSession
    UmiEditorHierarchyNavigationSession;

/**
 * Initialise editor hierarchy navigation session from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_hierarchy_navigation_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorHierarchyNavigationSession **out_session);
/**
 * Release or reset state held by editor hierarchy navigation session so the same storage
 * can be reused safely.
 */
void umi_editor_hierarchy_navigation_session_destroy(
    UmiEditorHierarchyNavigationSession *session);
/**
 * Provide the editor hierarchy navigation session open operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_open(
    UmiEditorHierarchyNavigationSession *session,
    const UmiEditorNavigationHierarchyRequest *request);
/**
 * Provide the editor hierarchy navigation session refresh operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_refresh(
    UmiEditorHierarchyNavigationSession *session);
/**
 * Provide the editor hierarchy navigation session cancel operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_cancel(
    UmiEditorHierarchyNavigationSession *session);
/**
 * Provide the editor hierarchy navigation session close operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_close(
    UmiEditorHierarchyNavigationSession *session);
/**
 * Provide the editor hierarchy navigation session expand operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_expand(
    UmiEditorHierarchyNavigationSession *session,
    const char *node_id);
/**
 * Provide the editor hierarchy navigation session collapse operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_collapse(
    UmiEditorHierarchyNavigationSession *session,
    const char *node_id);
/**
 * Provide the editor hierarchy navigation session expand all operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_expand_all(
    UmiEditorHierarchyNavigationSession *session);
/**
 * Provide the editor hierarchy navigation session collapse all operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_collapse_all(
    UmiEditorHierarchyNavigationSession *session);
/**
 * Provide the editor hierarchy navigation session set filter operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_set_filter(
    UmiEditorHierarchyNavigationSession *session,
    const char *filter);
/**
 * Provide the editor hierarchy navigation session select operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_select(
    UmiEditorHierarchyNavigationSession *session,
    const char *node_id);
/**
 * Provide the editor hierarchy navigation session select next operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_select_next(
    UmiEditorHierarchyNavigationSession *session,
    int wrap);
/**
 * Provide the editor hierarchy navigation session select previous operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_select_previous(
    UmiEditorHierarchyNavigationSession *session,
    int wrap);
/**
 * Find editor hierarchy navigation session while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_hierarchy_navigation_session_at(
    const UmiEditorHierarchyNavigationSession *session,
    size_t position,
    UmiEditorHierarchyNavigationEntry *out_entry);
/**
 * Find editor hierarchy navigation session visible while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_editor_hierarchy_navigation_session_visible_at(
    const UmiEditorHierarchyNavigationSession *session,
    size_t visible_position,
    UmiEditorHierarchyNavigationEntry *out_entry);
/**
 * Find editor hierarchy navigation session while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_hierarchy_navigation_session_selected(
    const UmiEditorHierarchyNavigationSession *session,
    UmiEditorHierarchyNavigationEntry *out_entry);
/**
 * Find editor hierarchy navigation session provider report while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_editor_hierarchy_navigation_session_provider_report_at(
    const UmiEditorHierarchyNavigationSession *session,
    size_t position,
    UmiEditorNavigationProviderReport *out_report);
/**
 * Provide the editor hierarchy navigation session snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_snapshot(
    const UmiEditorHierarchyNavigationSession *session,
    UmiEditorHierarchyNavigationSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor hierarchy navigation session without
 * changing their state.
 */
size_t umi_editor_hierarchy_navigation_session_count(
    const UmiEditorHierarchyNavigationSession *session);
/**
 * Return the number of records represented by editor hierarchy navigation session visible
 * without changing their state.
 */
size_t umi_editor_hierarchy_navigation_session_visible_count(
    const UmiEditorHierarchyNavigationSession *session);
/**
 * Provide the editor hierarchy navigation session revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_hierarchy_navigation_session_revision(
    const UmiEditorHierarchyNavigationSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_HIERARCHY_NAVIGATION_SESSION_H */
