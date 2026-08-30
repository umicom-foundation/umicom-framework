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

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int peek_query_kind(UmiEditorNavigationQueryKind query_kind)
{
    return query_kind == UMI_EDITOR_NAVIGATION_QUERY_DEFINITION ||
           query_kind == UMI_EDITOR_NAVIGATION_QUERY_DECLARATION ||
           query_kind == UMI_EDITOR_NAVIGATION_QUERY_TYPE_DEFINITION ||
           query_kind == UMI_EDITOR_NAVIGATION_QUERY_IMPLEMENTATION;
}

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
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_navigation_source_preview_cache_resolve(
        session->previews, &request, &session->active_preview);
    if (status == UMI_STATUS_OK) {
        session->has_preview = 1;
        session->stale = session->stale || session->active_preview.stale;
        return UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_UNAVAILABLE) {
        return UMI_STATUS_OK;
    }
    return status;
}

UmiStatus umi_editor_peek_navigation_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorPeekNavigationSession **out_session)
{
    UmiEditorPeekNavigationSession *session;
    UmiStatus status;

    if (registry == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_session = NULL;
    session = (UmiEditorPeekNavigationSession *)calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_navigation_query_session_create(registry,
                                                        &session->query);
    if (status == UMI_STATUS_OK) {
        status = umi_editor_navigation_source_preview_cache_create(
            registry, NULL, &session->previews);
    }
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

void umi_editor_peek_navigation_session_destroy(
    UmiEditorPeekNavigationSession *session)
{
    if (session == NULL) return;
    umi_editor_navigation_query_session_destroy(session->query);
    umi_editor_navigation_source_preview_cache_destroy(session->previews);
    session->query = NULL;
    session->previews = NULL;
    free(session);
}

UmiStatus umi_editor_peek_navigation_session_open(
    UmiEditorPeekNavigationSession *session,
    const UmiEditorNavigationRequest *request)
{
    UmiEditorNavigationQueryOptions options;
    UmiStatus status;

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
    if (status == UMI_STATUS_CANCELLED) {
        session->state = UMI_EDITOR_PEEK_NAVIGATION_CANCELLED;
        session->revision = next_revision(session->revision);
        return status;
    }
    if (status != UMI_STATUS_OK ||
        umi_editor_navigation_query_session_count(session->query) == 0U) {
        session->state = UMI_EDITOR_PEEK_NAVIGATION_FAILED;
        session->revision = next_revision(session->revision);
        return status != UMI_STATUS_OK ? status : UMI_STATUS_NOT_FOUND;
    }
    status = refresh_preview(session);
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

UmiStatus umi_editor_peek_navigation_session_refresh(
    UmiEditorPeekNavigationSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!session->has_request) return UMI_STATUS_INVALID_STATE;
    return umi_editor_peek_navigation_session_open(session, &session->request);
}

UmiStatus umi_editor_peek_navigation_session_cancel(
    UmiEditorPeekNavigationSession *session)
{
    UmiStatus status;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_query_session_cancel(session->query);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_INVALID_STATE) {
        return status;
    }
    session->state = UMI_EDITOR_PEEK_NAVIGATION_CANCELLED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_navigation_session_close(
    UmiEditorPeekNavigationSession *session,
    int force)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
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

UmiStatus umi_editor_peek_navigation_session_set_pinned(
    UmiEditorPeekNavigationSession *session,
    int pinned)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!session->visible) return UMI_STATUS_INVALID_STATE;
    session->pinned = pinned != 0;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_navigation_session_set_focus(
    UmiEditorPeekNavigationSession *session,
    UmiEditorPeekNavigationFocus focus)
{
    if (session == NULL ||
        (focus != UMI_EDITOR_PEEK_NAVIGATION_FOCUS_RESULTS &&
         focus != UMI_EDITOR_PEEK_NAVIGATION_FOCUS_SOURCE)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!session->visible) return UMI_STATUS_INVALID_STATE;
    session->focus = focus;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_navigation_session_select(
    UmiEditorPeekNavigationSession *session,
    size_t position)
{
    UmiStatus status;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_query_session_select(session->query, position);
    if (status != UMI_STATUS_OK) return status;
    status = refresh_preview(session);
    if (status != UMI_STATUS_OK) return status;
    session->state = session->stale ? UMI_EDITOR_PEEK_NAVIGATION_STALE
                                    : UMI_EDITOR_PEEK_NAVIGATION_OPEN;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_navigation_session_select_next(
    UmiEditorPeekNavigationSession *session,
    int wrap)
{
    UmiEditorNavigationQuerySessionSnapshot snapshot;
    UmiStatus status;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_query_session_snapshot(session->query,
                                                          &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!snapshot.has_selection) return UMI_STATUS_NOT_FOUND;
    if (!wrap && snapshot.selected_index + 1U >= snapshot.result_count) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_editor_navigation_query_session_select_next(session->query);
    if (status != UMI_STATUS_OK) return status;
    status = refresh_preview(session);
    if (status != UMI_STATUS_OK) return status;
    session->state = session->stale ? UMI_EDITOR_PEEK_NAVIGATION_STALE
                                    : UMI_EDITOR_PEEK_NAVIGATION_OPEN;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_navigation_session_select_previous(
    UmiEditorPeekNavigationSession *session,
    int wrap)
{
    UmiEditorNavigationQuerySessionSnapshot snapshot;
    UmiStatus status;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_query_session_snapshot(session->query,
                                                          &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!snapshot.has_selection) return UMI_STATUS_NOT_FOUND;
    if (!wrap && snapshot.selected_index == 0U) return UMI_STATUS_NOT_FOUND;
    status = umi_editor_navigation_query_session_select_previous(session->query);
    if (status != UMI_STATUS_OK) return status;
    status = refresh_preview(session);
    if (status != UMI_STATUS_OK) return status;
    session->state = session->stale ? UMI_EDITOR_PEEK_NAVIGATION_STALE
                                    : UMI_EDITOR_PEEK_NAVIGATION_OPEN;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_navigation_session_active_result(
    const UmiEditorPeekNavigationSession *session,
    UmiEditorNavigationResult *out_result)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_editor_navigation_query_session_selected(session->query,
                                                        out_result);
}

UmiStatus umi_editor_peek_navigation_session_active_preview(
    const UmiEditorPeekNavigationSession *session,
    UmiEditorNavigationSourcePreview *out_preview)
{
    if (session == NULL || out_preview == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!session->has_preview) return UMI_STATUS_NOT_FOUND;
    *out_preview = session->active_preview;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_peek_navigation_session_snapshot(
    const UmiEditorPeekNavigationSession *session,
    UmiEditorPeekNavigationSnapshot *out_snapshot)
{
    UmiEditorNavigationQuerySessionSnapshot query_snapshot;
    UmiStatus status;

    if (session == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_navigation_query_session_snapshot(session->query,
                                                          &query_snapshot);
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

UmiEditorNavigationQuerySession *umi_editor_peek_navigation_session_query(
    UmiEditorPeekNavigationSession *session)
{
    return session != NULL ? session->query : NULL;
}

uint64_t umi_editor_peek_navigation_session_revision(
    const UmiEditorPeekNavigationSession *session)
{
    return session != NULL ? session->revision : 0U;
}
