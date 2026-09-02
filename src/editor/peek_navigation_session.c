/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/peek_navigation_session.c
 *
 * PURPOSE:
 *   Implement the reusable peek-definition lifecycle above provider queries and
 *   source-preview resolution.  Pinning and focus are presentation-neutral
 *   state, while the actual source remains behind provider adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/peek_navigation_session.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorPeekNavigationSession {
    UmiEditorNavigationQuerySession *query;
    UmiEditorNavigationSourcePreviewCache *previews;
    UmiEditorNavigationRequest request;
    UmiEditorNavigationSourcePreview active_preview;
    UmiEditorPeekNavigationState state;
    UmiEditorPeekNavigationFocus focus;
    uint64_t revision;
    int has_request;
    int has_preview;
    int visible;
    int pinned;
    int stale;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the peek query kind operation used by this module and its client applications. */
static int peek_query_kind(UmiEditorNavigationQueryKind query_kind)
{
    return query_kind == UMI_EDITOR_NAVIGATION_QUERY_DEFINITION ||
           query_kind == UMI_EDITOR_NAVIGATION_QUERY_DECLARATION ||
           query_kind == UMI_EDITOR_NAVIGATION_QUERY_TYPE_DEFINITION ||
           query_kind == UMI_EDITOR_NAVIGATION_QUERY_IMPLEMENTATION;
}

/* Provide the refresh preview operation used by this module and its client applications. */
static UmiStatus refresh_preview(UmiEditorPeekNavigationSession *session)
{
    UmiEditorNavigationResult result;
    UmiEditorNavigationPreviewRequest request;
    uint64_t end_line;
    UmiStatus status;

    session->has_preview = 0;
    session->stale = 0;
    (void)memset(&session->active_preview, 0, sizeof(session->active_preview));
    status = umi_editor_navigation_query_session_selected(session->query,
                                                          &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->stale =
        strcmp(session->request.origin.uri, result.location.uri) == 0 &&
        session->request.document_revision != 0U &&
        result.location.document_revision != 0U &&
        session->request.document_revision !=
            result.location.document_revision;
    end_line = result.location.line > UINT64_MAX - 3U
        ? UINT64_MAX
        : result.location.line + 3U;
    status = umi_editor_navigation_preview_request_initialize(
        &request,
        session->request.request_id,
        result.location.uri,
        session->request.language_id,
        result.location.document_revision,
        result.location.line > 3U ? result.location.line - 3U : 0U,
        end_line);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_navigation_source_preview_cache_resolve(
        session->previews, &request, &session->active_preview);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        session->has_preview = 1;
        session->stale = session->stale || session->active_preview.stale;
        return UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_UNAVAILABLE) {
        return UMI_STATUS_OK;
    }
    return status;
}

/*
 * Initialise editor peek navigation session from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_peek_navigation_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorPeekNavigationSession **out_session)
{
    UmiEditorPeekNavigationSession *session;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_session = NULL;
    session = (UmiEditorPeekNavigationSession *)calloc(1U, sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_navigation_query_session_create(registry,
                                                        &session->query);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_navigation_source_preview_cache_create(
            registry, NULL, &session->previews);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_editor_navigation_query_session_destroy(session->query);
        free(session);
        return status;
    }
    session->state = UMI_EDITOR_PEEK_NAVIGATION_CLOSED;
    session->focus = UMI_EDITOR_PEEK_NAVIGATION_FOCUS_RESULTS;
    session->revision = 1U;
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor peek navigation session so the same storage can be
 * reused safely.
 */
void umi_editor_peek_navigation_session_destroy(
    UmiEditorPeekNavigationSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    umi_editor_navigation_query_session_destroy(session->query);
    umi_editor_navigation_source_preview_cache_destroy(session->previews);
    session->query = NULL;
    session->previews = NULL;
    free(session);
}

/*
 * Provide the editor peek navigation session open operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_navigation_session_open(
    UmiEditorPeekNavigationSession *session,
    const UmiEditorNavigationRequest *request)
{
    UmiEditorNavigationQueryOptions options;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || request == NULL || !peek_query_kind(request->query_kind)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->state = UMI_EDITOR_PEEK_NAVIGATION_LOADING;
    session->visible = 1;
    session->stale = 0;
    session->has_preview = 0;
    session->request = *request;
    session->has_request = 1;
    (void)umi_editor_navigation_query_options_initialize(&options);
    options.maximum_results = request->maximum_results > 0U
        ? request->maximum_results
        : UMI_EDITOR_NAVIGATION_DEFAULT_MAXIMUM_RESULTS;
    status = umi_editor_navigation_query_session_execute(session->query,
                                                         request,
                                                         &options);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CANCELLED) {
        session->state = UMI_EDITOR_PEEK_NAVIGATION_CANCELLED;
        session->revision = next_revision(session->revision);
        return status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK ||
        umi_editor_navigation_query_session_count(session->query) == 0U) {
        session->state = UMI_EDITOR_PEEK_NAVIGATION_FAILED;
        session->revision = next_revision(session->revision);
        return status != UMI_STATUS_OK ? status : UMI_STATUS_NOT_FOUND;
    }
    status = refresh_preview(session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        session->state = UMI_EDITOR_PEEK_NAVIGATION_FAILED;
        session->revision = next_revision(session->revision);
        return status;
    }
    session->state = session->stale ? UMI_EDITOR_PEEK_NAVIGATION_STALE
                                    : UMI_EDITOR_PEEK_NAVIGATION_OPEN;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek navigation session refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_navigation_session_refresh(
    UmiEditorPeekNavigationSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!session->has_request) return UMI_STATUS_INVALID_STATE;
    return umi_editor_peek_navigation_session_open(session, &session->request);
}

/*
 * Provide the editor peek navigation session cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_navigation_session_cancel(
    UmiEditorPeekNavigationSession *session)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_query_session_cancel(session->query);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK && status != UMI_STATUS_INVALID_STATE) {
        return status;
    }
    session->state = UMI_EDITOR_PEEK_NAVIGATION_CANCELLED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek navigation session close operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_navigation_session_close(
    UmiEditorPeekNavigationSession *session,
    int force)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->pinned && !force) return UMI_STATUS_BUSY;
    (void)umi_editor_navigation_query_session_clear(session->query);
    session->state = UMI_EDITOR_PEEK_NAVIGATION_CLOSED;
    session->visible = 0;
    session->pinned = 0;
    session->stale = 0;
    session->has_preview = 0;
    session->has_request = 0;
    (void)memset(&session->request, 0, sizeof(session->request));
    (void)memset(&session->active_preview, 0,
                 sizeof(session->active_preview));
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek navigation session set pinned operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_set_pinned(
    UmiEditorPeekNavigationSession *session,
    int pinned)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!session->visible) return UMI_STATUS_INVALID_STATE;
    session->pinned = pinned != 0;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek navigation session set focus operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_set_focus(
    UmiEditorPeekNavigationSession *session,
    UmiEditorPeekNavigationFocus focus)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL ||
        (focus != UMI_EDITOR_PEEK_NAVIGATION_FOCUS_RESULTS &&
         focus != UMI_EDITOR_PEEK_NAVIGATION_FOCUS_SOURCE)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!session->visible) return UMI_STATUS_INVALID_STATE;
    session->focus = focus;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek navigation session select operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_peek_navigation_session_select(
    UmiEditorPeekNavigationSession *session,
    size_t position)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_query_session_select(session->query, position);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = refresh_preview(session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->state = session->stale ? UMI_EDITOR_PEEK_NAVIGATION_STALE
                                    : UMI_EDITOR_PEEK_NAVIGATION_OPEN;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek navigation session select next operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_select_next(
    UmiEditorPeekNavigationSession *session,
    int wrap)
{
    UmiEditorNavigationQuerySessionSnapshot snapshot;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_query_session_snapshot(session->query,
                                                          &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!snapshot.has_selection) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!wrap && snapshot.selected_index + 1U >= snapshot.result_count) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_editor_navigation_query_session_select_next(session->query);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = refresh_preview(session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->state = session->stale ? UMI_EDITOR_PEEK_NAVIGATION_STALE
                                    : UMI_EDITOR_PEEK_NAVIGATION_OPEN;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek navigation session select previous operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_select_previous(
    UmiEditorPeekNavigationSession *session,
    int wrap)
{
    UmiEditorNavigationQuerySessionSnapshot snapshot;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_query_session_snapshot(session->query,
                                                          &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!snapshot.has_selection) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!wrap && snapshot.selected_index == 0U) return UMI_STATUS_NOT_FOUND;
    status = umi_editor_navigation_query_session_select_previous(session->query);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = refresh_preview(session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->state = session->stale ? UMI_EDITOR_PEEK_NAVIGATION_STALE
                                    : UMI_EDITOR_PEEK_NAVIGATION_OPEN;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek navigation session active result operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_active_result(
    const UmiEditorPeekNavigationSession *session,
    UmiEditorNavigationResult *out_result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_editor_navigation_query_session_selected(session->query,
                                                        out_result);
}

/*
 * Provide the editor peek navigation session active preview operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_active_preview(
    const UmiEditorPeekNavigationSession *session,
    UmiEditorNavigationSourcePreview *out_preview)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_preview == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!session->has_preview) return UMI_STATUS_NOT_FOUND;
    *out_preview = session->active_preview;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek navigation session snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_peek_navigation_session_snapshot(
    const UmiEditorPeekNavigationSession *session,
    UmiEditorPeekNavigationSnapshot *out_snapshot)
{
    UmiEditorNavigationQuerySessionSnapshot query_snapshot;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_navigation_query_session_snapshot(session->query,
                                                          &query_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_PEEK_NAVIGATION_SESSION_API_VERSION;
    out_snapshot->state = session->state;
    out_snapshot->focus = session->focus;
    out_snapshot->result_count = query_snapshot.result_count;
    out_snapshot->active_index = query_snapshot.selected_index;
    out_snapshot->request_id = session->has_request
        ? session->request.request_id
        : 0U;
    out_snapshot->document_revision = session->has_request
        ? session->request.document_revision
        : 0U;
    out_snapshot->revision = session->revision;
    out_snapshot->visible = session->visible;
    out_snapshot->pinned = session->pinned;
    out_snapshot->has_preview = session->has_preview;
    out_snapshot->stale = session->stale;
    out_snapshot->can_select_previous = query_snapshot.result_count > 1U;
    out_snapshot->can_select_next = query_snapshot.result_count > 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor peek navigation session query operation used by this module and its
 * client applications.
 */
UmiEditorNavigationQuerySession *umi_editor_peek_navigation_session_query(
    UmiEditorPeekNavigationSession *session)
{
    return session != NULL ? session->query : NULL;
}

/*
 * Provide the editor peek navigation session revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_peek_navigation_session_revision(
    const UmiEditorPeekNavigationSession *session)
{
    return session != NULL ? session->revision : 0U;
}
