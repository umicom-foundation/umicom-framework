/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/peek_navigation_session.h
 *
 * PURPOSE:
 *   Provide the complete toolkit-neutral peek lifecycle: provider query,
 *   result selection, source excerpt resolution, pinning, focus, refresh,
 *   stale-state reporting and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_PEEK_NAVIGATION_SESSION_H
#define UMICOM_EDITOR_PEEK_NAVIGATION_SESSION_H

#include "umicom/editor/navigation_query_session.h"
#include "umicom/editor/navigation_source_preview.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_PEEK_NAVIGATION_SESSION_API_VERSION 1U

/**
 * List the named editor peek navigation state values accepted by this public contract.
 */
typedef enum UmiEditorPeekNavigationState {
    UMI_EDITOR_PEEK_NAVIGATION_CLOSED = 1,
    UMI_EDITOR_PEEK_NAVIGATION_LOADING = 2,
    UMI_EDITOR_PEEK_NAVIGATION_OPEN = 3,
    UMI_EDITOR_PEEK_NAVIGATION_STALE = 4,
    UMI_EDITOR_PEEK_NAVIGATION_CANCELLED = 5,
    UMI_EDITOR_PEEK_NAVIGATION_FAILED = 6
} UmiEditorPeekNavigationState;

/**
 * List the named editor peek navigation focus values accepted by this public contract.
 */
typedef enum UmiEditorPeekNavigationFocus {
    UMI_EDITOR_PEEK_NAVIGATION_FOCUS_RESULTS = 1,
    UMI_EDITOR_PEEK_NAVIGATION_FOCUS_SOURCE = 2
} UmiEditorPeekNavigationFocus;

/**
 * Represent the editor peek navigation snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorPeekNavigationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorPeekNavigationState state;
    UmiEditorPeekNavigationFocus focus;
    size_t result_count;
    size_t active_index;
    uint64_t request_id;
    uint64_t document_revision;
    uint64_t revision;
    int visible;
    int pinned;
    int has_preview;
    int stale;
    int can_select_previous;
    int can_select_next;
} UmiEditorPeekNavigationSnapshot;

/**
 * Represent the editor peek navigation session data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorPeekNavigationSession
    UmiEditorPeekNavigationSession;

/**
 * Initialise editor peek navigation session from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_peek_navigation_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorPeekNavigationSession **out_session);
/**
 * Release or reset state held by editor peek navigation session so the same storage can be
 * reused safely.
 */
void umi_editor_peek_navigation_session_destroy(
    UmiEditorPeekNavigationSession *session);
/**
 * Provide the editor peek navigation session open operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_navigation_session_open(
    UmiEditorPeekNavigationSession *session,
    const UmiEditorNavigationRequest *request);
/**
 * Provide the editor peek navigation session refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_navigation_session_refresh(
    UmiEditorPeekNavigationSession *session);
/**
 * Provide the editor peek navigation session cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_navigation_session_cancel(
    UmiEditorPeekNavigationSession *session);
/**
 * Provide the editor peek navigation session close operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_navigation_session_close(
    UmiEditorPeekNavigationSession *session,
    int force);
/**
 * Provide the editor peek navigation session set pinned operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_set_pinned(
    UmiEditorPeekNavigationSession *session,
    int pinned);
/**
 * Provide the editor peek navigation session set focus operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_set_focus(
    UmiEditorPeekNavigationSession *session,
    UmiEditorPeekNavigationFocus focus);
/**
 * Provide the editor peek navigation session select operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_navigation_session_select(
    UmiEditorPeekNavigationSession *session,
    size_t position);
/**
 * Provide the editor peek navigation session select next operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_select_next(
    UmiEditorPeekNavigationSession *session,
    int wrap);
/**
 * Provide the editor peek navigation session select previous operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_select_previous(
    UmiEditorPeekNavigationSession *session,
    int wrap);
/**
 * Provide the editor peek navigation session active result operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_active_result(
    const UmiEditorPeekNavigationSession *session,
    UmiEditorNavigationResult *out_result);
/**
 * Provide the editor peek navigation session active preview operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_active_preview(
    const UmiEditorPeekNavigationSession *session,
    UmiEditorNavigationSourcePreview *out_preview);
/**
 * Provide the editor peek navigation session snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_snapshot(
    const UmiEditorPeekNavigationSession *session,
    UmiEditorPeekNavigationSnapshot *out_snapshot);
/**
 * Provide the editor peek navigation session query operation used by this module and its
 * client applications.
 */
UmiEditorNavigationQuerySession *umi_editor_peek_navigation_session_query(
    UmiEditorPeekNavigationSession *session);
/**
 * Provide the editor peek navigation session revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_peek_navigation_session_revision(
    const UmiEditorPeekNavigationSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_PEEK_NAVIGATION_SESSION_H */
