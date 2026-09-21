/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/close_session.c
 * PURPOSE:
 *   Sequence reviewed source closing without duplicating document ownership,
 *   saving or discard policy. Keep unanswered work after any stopped sequence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/close_session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct CloseTarget {
    UmiDocumentId id;
    char viewId[UMI_UI_ID_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    int hasPath;
} CloseTarget;

struct UmiDocumentCloseSession {
    UmiDocumentCoordinator *coordinator; /* Borrowed; owner-thread access only. */
    CloseTarget *targets;
    size_t next;
    int inOperation;
    UmiDocumentClosePlan *plan;          /* One owned reviewed draft at a time. */
    UmiDocumentCloseProgress progress;
};

static int CloseTerminal(UmiDocumentClosePhase phase)
{
    return phase == UMI_DOCUMENT_CLOSE_COMPLETE ||
        phase == UMI_DOCUMENT_CLOSE_FAILED || phase == UMI_DOCUMENT_CLOSE_CANCELLED;
}

/* The index is transient. Resolve stable identity after each earlier removal,
 * rather than treating a compacted coordinator array as the original list. */
static UmiStatus CloseTargetFind(UmiDocumentCloseSession *session,
    UmiDocumentWorkingCopySnapshot *outSnapshot, int *outAbsent)
{
    *outAbsent = 0;
    const CloseTarget *target = &session->targets[session->next];
    size_t count = umi_document_coordinator_count(session->coordinator);
    for (size_t index = 0U; index < count; ++index) {
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
    *outAbsent = 1;
    return UMI_STATUS_NOT_FOUND;
}

static UmiStatus CloseStop(UmiDocumentCloseSession *session, UmiStatus status)
{
    UmiDocumentClosePlanDestroy(session->plan);
    session->plan = NULL;
    session->progress.last_status = status;
    session->progress.phase = status == UMI_STATUS_CANCELLED
        ? UMI_DOCUMENT_CLOSE_CANCELLED : UMI_DOCUMENT_CLOSE_FAILED;
    session->inOperation = 0;
    return status;
}

static UmiStatus CloseAdvance(UmiDocumentCloseSession *session, int alreadyClosed)
{
    UmiDocumentClosePlanDestroy(session->plan);
    session->plan = NULL;
    if (alreadyClosed) ++session->progress.already_closed;
    else ++session->progress.closed;
    ++session->next;
    session->progress.remaining = session->progress.total - session->next;
    session->progress.last_status = UMI_STATUS_OK;
    session->progress.phase = session->progress.remaining == 0U
        ? UMI_DOCUMENT_CLOSE_COMPLETE : UMI_DOCUMENT_CLOSE_READY;
    session->inOperation = 0;
    return UMI_STATUS_OK;
}

UmiStatus UmiDocumentCloseSessionCreate(UmiDocumentCoordinator *coordinator,
    UmiDocumentCloseScope scope, UmiDocumentId keepDocument,
    UmiDocumentCloseSession **outSession)
{
    if (outSession == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSession = NULL;
    if (coordinator == NULL ||
        (scope != UMI_DOCUMENT_CLOSE_ALL && scope != UMI_DOCUMENT_CLOSE_OTHERS) ||
        (scope == UMI_DOCUMENT_CLOSE_ALL && keepDocument != 0U) ||
        (scope == UMI_DOCUMENT_CLOSE_OTHERS && keepDocument == 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t count = umi_document_coordinator_count(coordinator);
    if (count > UMI_DOCUMENT_MAX_WORKING_COPIES) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentCloseSession *session = calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->coordinator = coordinator;
    if (count != 0U) {
        session->targets = calloc(count, sizeof(*session->targets));
        if (session->targets == NULL) { free(session); return UMI_STATUS_OUT_OF_MEMORY; }
    }
    int keepFound = scope == UMI_DOCUMENT_CLOSE_ALL;
    for (size_t index = 0U; index < count; ++index) {
        UmiDocumentWorkingCopySnapshot snapshot;
        UmiStatus status = umi_document_coordinator_at(coordinator, index, &snapshot);
        if (status != UMI_STATUS_OK) { UmiDocumentCloseSessionDestroy(session); return status; }
        if (scope == UMI_DOCUMENT_CLOSE_OTHERS && snapshot.document_id == keepDocument) {
            keepFound = 1;
            continue;
        }
        CloseTarget *target = &session->targets[session->progress.total++];
        target->id = snapshot.document_id;
        target->hasPath = snapshot.has_path;
        memcpy(target->viewId, snapshot.view_id, sizeof(target->viewId));
        memcpy(target->path, snapshot.path, sizeof(target->path));
    }
    if (!keepFound) { UmiDocumentCloseSessionDestroy(session); return UMI_STATUS_NOT_FOUND; }
    session->progress.remaining = session->progress.total;
    session->progress.phase = count == (scope == UMI_DOCUMENT_CLOSE_OTHERS ? 1U : 0U)
        ? UMI_DOCUMENT_CLOSE_COMPLETE : UMI_DOCUMENT_CLOSE_READY;
    session->progress.last_status = UMI_STATUS_OK;
    *outSession = session;
    return UMI_STATUS_OK;
}

void UmiDocumentCloseSessionDestroy(UmiDocumentCloseSession *session)
{
    if (session == NULL) return;
    UmiDocumentClosePlanDestroy(session->plan);
    free(session->targets);
    free(session);
}

UmiStatus UmiDocumentCloseSessionProgress(const UmiDocumentCloseSession *session,
    UmiDocumentCloseProgress *outProgress)
{
    if (session == NULL || outProgress == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outProgress = session->progress;
    return UMI_STATUS_OK;
}

UmiStatus UmiDocumentCloseSessionStep(UmiDocumentCloseSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->inOperation) return UMI_STATUS_BUSY;
    if (CloseTerminal(session->progress.phase)) return session->progress.last_status;
    if (session->progress.phase != UMI_DOCUMENT_CLOSE_READY) return UMI_STATUS_OK;
    session->inOperation = 1;
    memset(&session->progress.current, 0, sizeof(session->progress.current));
    session->progress.current.document_id = session->targets[session->next].id;
    UmiDocumentWorkingCopySnapshot snapshot;
    int absent = 0;
    UmiStatus status = CloseTargetFind(session, &snapshot, &absent);
    if (absent) return CloseAdvance(session, 1);
    if (status != UMI_STATUS_OK) return CloseStop(session, status);
    status = UmiDocumentCoordinatorPrepareClose(session->coordinator,
        snapshot.document_id, &session->plan);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentClosePlanSummary(session->plan, &session->progress.current);
    if (status != UMI_STATUS_OK) return CloseStop(session, status);
    if (session->progress.current.dirty) {
        session->progress.phase = UMI_DOCUMENT_CLOSE_DECISION;
        session->inOperation = 0;
        return UMI_STATUS_OK;
    }
    status = UmiDocumentCoordinatorApplyClose(session->coordinator, session->plan,
        UMI_DOCUMENT_CLOSE_UNMODIFIED, NULL);
    return status == UMI_STATUS_OK ? CloseAdvance(session, 0) : CloseStop(session, status);
}

UmiStatus UmiDocumentCloseSessionCheck(const UmiDocumentCloseSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->inOperation) return UMI_STATUS_BUSY;
    if (session->plan == NULL ||
        (session->progress.phase != UMI_DOCUMENT_CLOSE_DECISION &&
         session->progress.phase != UMI_DOCUMENT_CLOSE_PATH)) return UMI_STATUS_INVALID_STATE;
    return UmiDocumentCoordinatorCheckClose(session->coordinator, session->plan);
}

UmiStatus UmiDocumentCloseSessionRespond(UmiDocumentCloseSession *session,
    UmiDocumentCloseDecision decision, const char *saveAsPath)
{
    if (session == NULL || decision < UMI_DOCUMENT_CLOSE_UNMODIFIED ||
        decision > UMI_DOCUMENT_CLOSE_CANCEL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->inOperation) return UMI_STATUS_BUSY;
    if (CloseTerminal(session->progress.phase)) return UMI_STATUS_INVALID_STATE;
    if (decision != UMI_DOCUMENT_CLOSE_SAVE && saveAsPath != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (decision == UMI_DOCUMENT_CLOSE_CANCEL) {
        (void)UmiDocumentCloseSessionCancel(session);
        return UMI_STATUS_CANCELLED;
    }
    if (session->plan == NULL || (session->progress.phase != UMI_DOCUMENT_CLOSE_DECISION &&
        session->progress.phase != UMI_DOCUMENT_CLOSE_PATH)) return UMI_STATUS_INVALID_STATE;
    if (decision == UMI_DOCUMENT_CLOSE_SAVE && saveAsPath != NULL) {
        if (session->progress.current.has_path) return UMI_STATUS_INVALID_ARGUMENT;
        size_t length = 0U;
        while (length < UMI_PATH_CAPACITY && saveAsPath[length] != '\0') ++length;
        if (length == UMI_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (length == 0U || !umi_path_is_absolute(saveAsPath)) return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = UmiDocumentCloseSessionCheck(session);
    if (status != UMI_STATUS_OK) return CloseStop(session, status);
    if (decision == UMI_DOCUMENT_CLOSE_SAVE && !session->progress.current.has_path && saveAsPath == NULL) {
        if (session->progress.current.read_only) return CloseStop(session, UMI_STATUS_PERMISSION_DENIED);
        session->progress.phase = UMI_DOCUMENT_CLOSE_PATH;
        return UMI_STATUS_OK;
    }
    session->inOperation = 1;
    status = UmiDocumentCoordinatorApplyClose(session->coordinator, session->plan, decision, saveAsPath);
    return status == UMI_STATUS_OK ? CloseAdvance(session, 0) : CloseStop(session, status);
}

UmiStatus UmiDocumentCloseSessionCancel(UmiDocumentCloseSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->inOperation) return UMI_STATUS_BUSY;
    if (!CloseTerminal(session->progress.phase)) (void)CloseStop(session, UMI_STATUS_CANCELLED);
    return UMI_STATUS_OK;
}

/* A progress snapshot is copied across a presentation boundary. Validate the
 * snapshot itself before another component displays it or claims completion.
 * This check never reads a document, consumes a decision or closes a source. */
UmiStatus UmiDocumentCloseProgressValidate(const UmiDocumentCloseProgress *progress)
{
    if (progress == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (progress->phase < UMI_DOCUMENT_CLOSE_READY || progress->phase > UMI_DOCUMENT_CLOSE_CANCELLED ||
        progress->last_status < UMI_STATUS_OK || progress->last_status > UMI_STATUS_BUSY ||
        memchr(progress->current.display_name, '\0', sizeof(progress->current.display_name)) == NULL ||
        progress->closed > progress->total || progress->already_closed > progress->total - progress->closed ||
        progress->remaining != progress->total - progress->closed - progress->already_closed ||
        (progress->phase == UMI_DOCUMENT_CLOSE_COMPLETE && progress->remaining != 0U) ||
        (progress->phase <= UMI_DOCUMENT_CLOSE_PATH && progress->remaining == 0U) ||
        (progress->phase == UMI_DOCUMENT_CLOSE_CANCELLED && progress->last_status != UMI_STATUS_CANCELLED) ||
        (progress->phase == UMI_DOCUMENT_CLOSE_FAILED && progress->last_status == UMI_STATUS_OK) ||
        (progress->phase <= UMI_DOCUMENT_CLOSE_COMPLETE && progress->last_status != UMI_STATUS_OK))
        return UMI_STATUS_INVALID_ARGUMENT;
    if ((progress->current.dirty != 0 && progress->current.dirty != 1) ||
        (progress->current.has_path != 0 && progress->current.has_path != 1) ||
        (progress->current.read_only != 0 && progress->current.read_only != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* Returning OK from Step means the step ran. Only a complete, valid progress
 * snapshot establishes that all captured targets have been accounted for. */
UmiStatus UmiDocumentCloseProgressRequireComplete(const UmiDocumentCloseProgress *progress)
{
    UmiStatus status = UmiDocumentCloseProgressValidate(progress);
    if (status != UMI_STATUS_OK) return status;
    if (progress->phase == UMI_DOCUMENT_CLOSE_COMPLETE) return UMI_STATUS_OK;
    if (progress->phase == UMI_DOCUMENT_CLOSE_FAILED ||
        progress->phase == UMI_DOCUMENT_CLOSE_CANCELLED) return progress->last_status;
    return UMI_STATUS_INVALID_STATE;
}

UmiStatus UmiDocumentCloseProgressFormat(const UmiDocumentCloseProgress *progress,
    char *text, size_t capacity)
{
    if (progress == NULL || text == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Validation moved to UmiDocumentCloseProgressValidate() in this same
     * Framework file so the GTK accessor and non-GUI consumers share it.
     * Former in-place checks retained for the migration record:
     *     if (progress->phase < UMI_DOCUMENT_CLOSE_READY || progress->phase > UMI_DOCUMENT_CLOSE_CANCELLED ||
     *         progress->last_status < UMI_STATUS_OK || progress->last_status > UMI_STATUS_BUSY ||
     *         memchr(progress->current.display_name, '\0', sizeof(progress->current.display_name)) == NULL ||
     *         progress->closed > progress->total || progress->already_closed > progress->total - progress->closed ||
     *         progress->remaining != progress->total - progress->closed - progress->already_closed ||
     *         (progress->phase == UMI_DOCUMENT_CLOSE_COMPLETE && progress->remaining != 0U) ||
     *         (progress->phase <= UMI_DOCUMENT_CLOSE_PATH && progress->remaining == 0U) ||
     *         (progress->phase == UMI_DOCUMENT_CLOSE_CANCELLED && progress->last_status != UMI_STATUS_CANCELLED) ||
     *         (progress->phase == UMI_DOCUMENT_CLOSE_FAILED && progress->last_status == UMI_STATUS_OK) ||
     *         (progress->phase <= UMI_DOCUMENT_CLOSE_COMPLETE && progress->last_status != UMI_STATUS_OK))
     *         return UMI_STATUS_INVALID_ARGUMENT;
     */
    UmiStatus status = UmiDocumentCloseProgressValidate(progress);
    if (status != UMI_STATUS_OK) return status;
    const char *state;
    switch (progress->phase) {
    case UMI_DOCUMENT_CLOSE_READY: state = "in progress"; break;
    case UMI_DOCUMENT_CLOSE_DECISION: state = "awaiting a decision"; break;
    case UMI_DOCUMENT_CLOSE_PATH: state = "awaiting a filename"; break;
    case UMI_DOCUMENT_CLOSE_COMPLETE: state = "complete"; break;
    case UMI_DOCUMENT_CLOSE_FAILED: state = "stopped"; break;
    case UMI_DOCUMENT_CLOSE_CANCELLED: state = "cancelled"; break;
    default: return UMI_STATUS_INVALID_ARGUMENT;
    }
    char message[UMI_DOCUMENT_NAME_CAPACITY + 384U];
    int length = snprintf(message, sizeof(message),
        "Close documents %s: %zu closed, %zu already closed, %zu remaining.%s%s%s%s",
        state, progress->closed, progress->already_closed, progress->remaining,
        progress->current.display_name[0] != '\0' ? " " : "", progress->current.display_name,
        progress->phase == UMI_DOCUMENT_CLOSE_FAILED ? ": " : "",
        progress->phase == UMI_DOCUMENT_CLOSE_FAILED ? umi_status_text(progress->last_status) : "");
    if (length < 0) return UMI_STATUS_IO_ERROR;
    if ((size_t)length >= sizeof(message) || (size_t)length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(text, message, (size_t)length + 1U);
    return UMI_STATUS_OK;
}
