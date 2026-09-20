/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/save_session.c
 *
 * PURPOSE:
 *   Sequence Save All through the existing document coordinator. This module
 *   owns progress and captured identities, not text storage or file writing.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/save_session.h"
#include "umicom/document/edit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SaveTarget {
    UmiDocumentId id;
    char viewId[UMI_UI_ID_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    int hasPath;
} SaveTarget;

struct UmiDocumentSaveSession {
    UmiDocumentCoordinator *coordinator; /* Borrowed for the session lifetime. */
    SaveTarget *targets;
    size_t next;
    int inStep;
    int cancelRequested;
    UmiDocumentSaveProgress progress;
};

/* Identity is independent of array position: closing another tab may compact
 * the coordinator. Always find the original document ID again. */
static UmiStatus CurrentTarget(UmiDocumentSaveSession *session,
    UmiDocumentWorkingCopySnapshot *outSnapshot)
{
    const SaveTarget *target = &session->targets[session->next];
    for (size_t index = 0U; index < umi_document_coordinator_count(session->coordinator); ++index) {
        UmiDocumentWorkingCopySnapshot snapshot;
        UmiStatus status = umi_document_coordinator_at(session->coordinator, index, &snapshot);
        if (status != UMI_STATUS_OK) return status;
        if (snapshot.document_id != target->id) continue;
        if (snapshot.has_path != target->hasPath ||
            strcmp(snapshot.path, target->path) != 0 ||
            strcmp(snapshot.view_id, target->viewId) != 0)
            return UMI_STATUS_INVALID_STATE;
        *outSnapshot = snapshot;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

static int Terminal(UmiDocumentSavePhase phase)
{
    return phase == UMI_DOCUMENT_SAVE_COMPLETE || phase == UMI_DOCUMENT_SAVE_FAILED ||
        phase == UMI_DOCUMENT_SAVE_CANCELLED;
}

/* Complete a single attempt. Never count a failed provider call as saved;
 * providers may have uncertain side effects and the error stays observable. */
static UmiStatus FinishStep(UmiDocumentSaveSession *session, UmiStatus status,
    int saved, int unchanged)
{
    session->inStep = 0;
    session->progress.last_status = status;
    if (status != UMI_STATUS_OK) {
        session->progress.phase = UMI_DOCUMENT_SAVE_FAILED;
        return status;
    }
    if (saved || unchanged) {
        session->progress.saved += saved ? 1U : 0U;
        session->progress.unchanged += unchanged ? 1U : 0U;
        ++session->next;
        session->progress.remaining = session->progress.total - session->next;
        session->progress.phase = session->next == session->progress.total
            ? UMI_DOCUMENT_SAVE_COMPLETE : UMI_DOCUMENT_SAVE_READY;
    }
    if (session->cancelRequested && session->progress.phase != UMI_DOCUMENT_SAVE_COMPLETE) {
        session->progress.phase = UMI_DOCUMENT_SAVE_CANCELLED;
        session->progress.last_status = UMI_STATUS_CANCELLED;
        return UMI_STATUS_CANCELLED;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiDocumentSaveSessionCreate(UmiDocumentCoordinator *coordinator,
    UmiDocumentSaveSession **outSession)
{
    if (outSession == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSession = NULL;
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t count = umi_document_coordinator_count(coordinator);
    if (count > UMI_DOCUMENT_MAX_WORKING_COPIES) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentSaveSession *session = calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (count != 0U) {
        session->targets = calloc(count, sizeof(*session->targets));
        if (session->targets == NULL) { free(session); return UMI_STATUS_OUT_OF_MEMORY; }
    }
    session->coordinator = coordinator;
    for (size_t index = 0U; index < count; ++index) {
        UmiDocumentWorkingCopySnapshot snapshot;
        UmiStatus status = umi_document_coordinator_at(coordinator, index, &snapshot);
        if (status != UMI_STATUS_OK) { UmiDocumentSaveSessionDestroy(session); return status; }
        if (!snapshot.dirty) continue;
        SaveTarget *target = &session->targets[session->progress.total++];
        target->id = snapshot.document_id;
        target->hasPath = snapshot.has_path;
        (void)snprintf(target->viewId, sizeof(target->viewId), "%s", snapshot.view_id);
        (void)snprintf(target->path, sizeof(target->path), "%s", snapshot.path);
    }
    session->progress.remaining = session->progress.total;
    session->progress.phase = session->progress.total == 0U
        ? UMI_DOCUMENT_SAVE_COMPLETE : UMI_DOCUMENT_SAVE_READY;
    session->progress.last_status = UMI_STATUS_OK;
    *outSession = session;
    return UMI_STATUS_OK;
}

void UmiDocumentSaveSessionDestroy(UmiDocumentSaveSession *session)
{
    if (session == NULL) return;
    free(session->targets);
    free(session);
}

UmiStatus UmiDocumentSaveSessionProgress(const UmiDocumentSaveSession *session,
    UmiDocumentSaveProgress *outProgress)
{
    if (session == NULL || outProgress == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outProgress = session->progress;
    return UMI_STATUS_OK;
}

UmiStatus UmiDocumentSaveSessionStep(UmiDocumentSaveSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->inStep) return UMI_STATUS_BUSY;
    if (Terminal(session->progress.phase)) return session->progress.last_status;
    if (session->progress.phase == UMI_DOCUMENT_SAVE_NEEDS_PATH) return UMI_STATUS_OK;
    session->inStep = 1;
    session->progress.current_document = session->targets[session->next].id;
    session->progress.display_name[0] = '\0';
    UmiDocumentWorkingCopySnapshot snapshot;
    UmiStatus status = CurrentTarget(session, &snapshot);
    if (status != UMI_STATUS_OK) return FinishStep(session, status, 0, 0);
    (void)snprintf(session->progress.display_name, sizeof(session->progress.display_name),
        "%s", snapshot.display_name);
    if (!snapshot.dirty) return FinishStep(session, UMI_STATUS_OK, 0, 1);
    UmiDocumentEditState edit;
    status = UmiDocumentCoordinatorGetEditState(session->coordinator, snapshot.document_id, &edit);
    if (status == UMI_STATUS_OK && edit.read_only) status = UMI_STATUS_PERMISSION_DENIED;
    if (status != UMI_STATUS_OK) return FinishStep(session, status, 0, 0);
    if (!snapshot.has_path) {
        session->progress.phase = UMI_DOCUMENT_SAVE_NEEDS_PATH;
        return FinishStep(session, UMI_STATUS_OK, 0, 0);
    }
    status = UmiDocumentCoordinatorSaveAs(session->coordinator, snapshot.document_id, snapshot.path);
    return FinishStep(session, status, status == UMI_STATUS_OK, 0);
}

UmiStatus UmiDocumentSaveSessionProvidePath(UmiDocumentSaveSession *session,
    const char *path)
{
    if (session == NULL || path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (session->inStep) return UMI_STATUS_BUSY;
    if (session->progress.phase != UMI_DOCUMENT_SAVE_NEEDS_PATH) return UMI_STATUS_INVALID_STATE;
    size_t length = 0U;
    while (length < UMI_PATH_CAPACITY && path[length] != '\0') ++length;
    if (length == UMI_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!umi_path_is_absolute(path)) return UMI_STATUS_INVALID_ARGUMENT;
    session->inStep = 1;
    UmiDocumentWorkingCopySnapshot snapshot;
    UmiStatus status = CurrentTarget(session, &snapshot);
    UmiDocumentEditState edit;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentCoordinatorGetEditState(session->coordinator, snapshot.document_id, &edit);
    if (status == UMI_STATUS_OK && edit.read_only) status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentCoordinatorSaveAs(session->coordinator, snapshot.document_id, path);
    return FinishStep(session, status, status == UMI_STATUS_OK, 0);
}

UmiStatus UmiDocumentSaveSessionCancel(UmiDocumentSaveSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (Terminal(session->progress.phase)) return UMI_STATUS_OK;
    session->cancelRequested = 1;
    if (!session->inStep) {
        session->progress.phase = UMI_DOCUMENT_SAVE_CANCELLED;
        session->progress.last_status = UMI_STATUS_CANCELLED;
    }
    return UMI_STATUS_OK;
}


/* The copied progress is a public boundary. Validate it before formatting;
 * subtracting each component avoids overflowing saved + unchanged + remaining. */
UmiStatus UmiDocumentSaveProgressFormat(const UmiDocumentSaveProgress *progress,
    char *text, size_t capacity)
{
    if (progress == NULL || text == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(progress->display_name, '\0', sizeof(progress->display_name)) == NULL ||
        progress->phase < UMI_DOCUMENT_SAVE_READY || progress->phase > UMI_DOCUMENT_SAVE_CANCELLED ||
        progress->saved > progress->total ||
        progress->unchanged > progress->total - progress->saved ||
        progress->remaining != progress->total - progress->saved - progress->unchanged)
        return UMI_STATUS_INVALID_ARGUMENT;
    if ((progress->phase == UMI_DOCUMENT_SAVE_CANCELLED && progress->last_status != UMI_STATUS_CANCELLED) ||
        (progress->phase == UMI_DOCUMENT_SAVE_FAILED && progress->last_status == UMI_STATUS_OK) ||
        ((progress->phase == UMI_DOCUMENT_SAVE_READY || progress->phase == UMI_DOCUMENT_SAVE_NEEDS_PATH ||
          progress->phase == UMI_DOCUMENT_SAVE_COMPLETE) && progress->last_status != UMI_STATUS_OK) ||
        (progress->phase == UMI_DOCUMENT_SAVE_COMPLETE && progress->remaining != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;

    char message[UMI_DOCUMENT_NAME_CAPACITY + 384U];
    int length;
    switch (progress->phase) {
    case UMI_DOCUMENT_SAVE_COMPLETE:
        length = snprintf(message, sizeof(message), "Save All complete: %zu saved, %zu already saved.",
            progress->saved, progress->unchanged);
        break;
    case UMI_DOCUMENT_SAVE_CANCELLED:
        length = snprintf(message, sizeof(message), "Save All cancelled: %zu saved, %zu already saved, %zu remaining.",
            progress->saved, progress->unchanged, progress->remaining);
        break;
    case UMI_DOCUMENT_SAVE_NEEDS_PATH:
        length = snprintf(message, sizeof(message), "Choose a filename for %s. %zu saved, %zu remaining.",
            progress->display_name, progress->saved, progress->remaining);
        break;
    case UMI_DOCUMENT_SAVE_FAILED:
        length = snprintf(message, sizeof(message), "Save All stopped: %zu saved, %zu remaining. %s%s%s",
            progress->saved, progress->remaining, progress->display_name,
            progress->display_name[0] != '\0' ? ": " : "", umi_status_text(progress->last_status));
        break;
    case UMI_DOCUMENT_SAVE_READY:
        length = snprintf(message, sizeof(message), "Save All: %zu saved, %zu remaining.",
            progress->saved, progress->remaining);
        break;
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length < 0) return UMI_STATUS_IO_ERROR;
    if ((size_t)length >= sizeof(message) || (size_t)length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Assemble privately, then publish a complete message including its zero. */
    memcpy(text, message, (size_t)length + 1U);
    return UMI_STATUS_OK;
}
