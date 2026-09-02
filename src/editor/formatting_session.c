/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/formatting_session.c
 *
 * PURPOSE:
 *   Implement formatting request lifecycle, conflict detection and safe edit
 *   application using the shared Framework workspace-edit engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/formatting_session.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorFormattingSession {
    UmiEditorWorkspaceEditSet *edit_set;
    UmiEditorFormattingRequest request;
    UmiEditorFormattingSessionState state;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the valid mode operation used by this module and its client applications. */
static int valid_mode(UmiEditorFormattingRequestMode mode)
{
    return mode >= UMI_EDITOR_FORMATTING_DOCUMENT &&
           mode <= UMI_EDITOR_FORMATTING_ON_PASTE;
}

/* Provide the validate request operation used by this module and its client applications. */
static UmiStatus validate_request(const UmiEditorFormattingRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_EDITOR_FORMATTING_SESSION_API_VERSION ||
        !terminated(request->id, sizeof(request->id)) ||
        request->id[0] == '\0' ||
        !terminated(request->provider_id, sizeof(request->provider_id)) ||
        request->provider_id[0] == '\0' ||
        !terminated(request->document_uri, sizeof(request->document_uri)) ||
        request->document_uri[0] == '\0' ||
        !terminated(request->profile_id, sizeof(request->profile_id)) ||
        request->profile_id[0] == '\0' || !valid_mode(request->mode) ||
        request->end_byte_offset < request->start_byte_offset) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->mode == UMI_EDITOR_FORMATTING_ON_TYPE &&
        request->trigger_character == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((request->mode == UMI_EDITOR_FORMATTING_RANGE ||
         request->mode == UMI_EDITOR_FORMATTING_ON_PASTE) &&
        request->end_byte_offset == request->start_byte_offset) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise editor formatting session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_formatting_session_create(
    UmiEditorFormattingSession **out_session)
{
    UmiEditorFormattingSession *session;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_session = NULL;
    session = (UmiEditorFormattingSession *)calloc(1U, sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_workspace_edit_set_create(&session->edit_set);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    session->revision = 1U;
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor formatting session so the same storage can be
 * reused safely.
 */
void umi_editor_formatting_session_destroy(UmiEditorFormattingSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    umi_editor_workspace_edit_set_destroy(session->edit_set);
    session->edit_set = NULL;
    free(session);
}

/*
 * Provide the editor formatting session begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_formatting_session_begin(
    UmiEditorFormattingSession *session,
    const UmiEditorFormattingRequest *request)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || validate_request(request) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_workspace_edit_set_clear(session->edit_set);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->request = *request;
    session->request.require_matching_revision =
        request->require_matching_revision != 0;
    session->state = UMI_EDITOR_FORMATTING_COLLECTING;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor formatting session add edit operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_formatting_session_add_edit(
    UmiEditorFormattingSession *session,
    const UmiEditorWorkspaceTextEdit *edit)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || edit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->state != UMI_EDITOR_FORMATTING_COLLECTING) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(edit->location.uri, session->request.document_uri) != 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->request.document_revision != 0U &&
        edit->location.document_revision != 0U &&
        edit->location.document_revision != session->request.document_revision) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_workspace_edit_set_upsert(session->edit_set, edit);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        session->revision = next_revision(session->revision);
    }
    return status;
}

/*
 * Provide the editor formatting session finalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_formatting_session_finalize(
    UmiEditorFormattingSession *session)
{
    UmiEditorWorkspaceEditSnapshot snapshot;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->state != UMI_EDITOR_FORMATTING_COLLECTING) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_editor_workspace_edit_set_finalize(session->edit_set);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        session->state = UMI_EDITOR_FORMATTING_FAILED;
        return status;
    }
    status = umi_editor_workspace_edit_set_snapshot(session->edit_set,
                                                    &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        session->state = UMI_EDITOR_FORMATTING_FAILED;
        return status;
    }
    session->state = snapshot.conflict_count > 0U
        ? UMI_EDITOR_FORMATTING_CONFLICT
        : UMI_EDITOR_FORMATTING_READY;
    session->revision = next_revision(session->revision);
    return snapshot.conflict_count > 0U ? UMI_STATUS_INVALID_STATE
                                        : UMI_STATUS_OK;
}

/*
 * Perform editor formatting session through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_editor_formatting_session_apply(
    UmiEditorFormattingSession *session,
    UmiEditorTextBuffer *buffer,
    size_t *out_applied_count)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || buffer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (session->state != UMI_EDITOR_FORMATTING_READY) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_editor_workspace_edit_set_apply_document(
        session->edit_set, session->request.document_uri, buffer,
        session->request.require_matching_revision, out_applied_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        session->state = UMI_EDITOR_FORMATTING_APPLIED;
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_INVALID_STATE) {
        session->state = UMI_EDITOR_FORMATTING_CONFLICT;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        session->state = UMI_EDITOR_FORMATTING_FAILED;
    }
    session->revision = next_revision(session->revision);
    return status;
}

/*
 * Provide the editor formatting session cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_formatting_session_cancel(
    UmiEditorFormattingSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->state == UMI_EDITOR_FORMATTING_APPLIED) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->state = UMI_EDITOR_FORMATTING_CANCELLED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Copy editor formatting session edit into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiEditorWorkspaceEditSet *umi_editor_formatting_session_edit_set(
    UmiEditorFormattingSession *session)
{
    return session != NULL ? session->edit_set : NULL;
}

/*
 * Provide the editor formatting session snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_formatting_session_snapshot(
    const UmiEditorFormattingSession *session,
    UmiEditorFormattingSessionSnapshot *out_snapshot)
{
    UmiEditorWorkspaceEditSnapshot edit_snapshot;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_workspace_edit_set_snapshot(session->edit_set,
                                               &edit_snapshot) !=
        UMI_STATUS_OK) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_FORMATTING_SESSION_API_VERSION;
    out_snapshot->request = session->request;
    out_snapshot->state = session->state;
    out_snapshot->edit_count = edit_snapshot.edit_count;
    out_snapshot->conflict_count = edit_snapshot.conflict_count;
    out_snapshot->applied_count = edit_snapshot.applied_count;
    out_snapshot->revision = session->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor formatting session revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_formatting_session_revision(
    const UmiEditorFormattingSession *session)
{
    return session != NULL ? session->revision : 0U;
}
