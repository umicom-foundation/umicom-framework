/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/replacement_session.c
 * PURPOSE: Sequence existing replacement captures without duplicating document or undo ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/replacement_session.h"
#include "search_options_internal.h"
#include "umicom/document/text_encoding.h"
#include <stdlib.h>
#include <string.h>

typedef struct ReplacementTarget {
    UmiDocumentId id;
    char view_id[UMI_UI_ID_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    int has_path;
} ReplacementTarget;

struct UmiDocumentReplacementSession {
    UmiDocumentCoordinator *coordinator;
    ReplacementTarget *targets;
    size_t next;
    char *needle;
    char *replacement;
    UmiDocumentReplacementPlan *plan;
    UmiDocumentReplacementProgress progress;
    UmiEditorSearchOptions searchOptions;
};

/* Reuse the document's byte ceiling for owned query text. Validate UTF-8 before
 * showing it in a native text view or retaining it for a later document. */
static UmiStatus ReplacementSessionInput(const char *text, int allow_empty, size_t *out_bytes)
{
    size_t bytes = 0U;
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    while (bytes <= UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES && text[bytes] != '\0') ++bytes;
    if (bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if ((!allow_empty && bytes == 0U) ||
        !umi_document_utf8_validate((const unsigned char *)text, bytes, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_bytes = bytes;
    return UMI_STATUS_OK;
}

static int ReplacementSessionTerminal(UmiDocumentReplacementPhase phase)
{
    return phase == UMI_DOCUMENT_REPLACEMENT_COMPLETE ||
        phase == UMI_DOCUMENT_REPLACEMENT_CANCELLED || phase == UMI_DOCUMENT_REPLACEMENT_FAILED;
}

/* Ownership is deliberately independent of the coordinator. Retained progress
 * can still be inspected and destroyed after its host closes. */
void UmiDocumentReplacementSessionDestroy(UmiDocumentReplacementSession *session)
{
    if (session == NULL) return;
    UmiDocumentReplacementPlanDestroy(session->plan);
    free(session->targets);
    free(session->needle);
    free(session->replacement);
    free(session);
}

/* All captured drafts now share a copied explicit search policy; the original entry point continues to choose smart case.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiDocumentReplacementSessionCreate(UmiDocumentCoordinator *coordinator,
    const char *needle, const char *replacement, UmiDocumentReplacementSession **outSession)
{
    size_t needle_bytes = 0U, replacement_bytes = 0U;
    if (outSession == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSession = NULL;
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ReplacementSessionInput(needle, 0, &needle_bytes);
    if (status == UMI_STATUS_OK) status = ReplacementSessionInput(replacement, 1, &replacement_bytes);
    if (status != UMI_STATUS_OK) return status;
    size_t count = umi_document_coordinator_count(coordinator);
    if (count > UMI_DOCUMENT_MAX_WORKING_COPIES) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentReplacementSession *session = calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->coordinator = coordinator;
    session->needle = malloc(needle_bytes + 1U);
    session->replacement = malloc(replacement_bytes + 1U);
    if (count != 0U) session->targets = calloc(count, sizeof(*session->targets));
    if (session->needle == NULL || session->replacement == NULL || (count != 0U && session->targets == NULL)) {
        UmiDocumentReplacementSessionDestroy(session); return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(session->needle, needle, needle_bytes + 1U);
    memcpy(session->replacement, replacement, replacement_bytes + 1U);
    for (size_t index = 0U; index < count; ++index) {
        UmiDocumentWorkingCopySnapshot snapshot;
        status = umi_document_coordinator_at(coordinator, index, &snapshot);
        if (status != UMI_STATUS_OK) { UmiDocumentReplacementSessionDestroy(session); return status; }
        ReplacementTarget *target = &session->targets[index];
        target->id = snapshot.document_id;
        target->has_path = snapshot.has_path;
        memcpy(target->view_id, snapshot.view_id, sizeof(target->view_id));
        memcpy(target->path, snapshot.path, sizeof(target->path));
    }
    session->progress.total = count;
    session->progress.remaining = count;
    session->progress.phase = count == 0U ? UMI_DOCUMENT_REPLACEMENT_COMPLETE : UMI_DOCUMENT_REPLACEMENT_READY;
    session->progress.last_status = UMI_STATUS_OK;
    *outSession = session;
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiDocumentReplacementSessionCreateWithOptions(UmiDocumentCoordinator *coordinator,
    const char *needle, const char *replacement, const UmiEditorSearchOptions *searchOptions, UmiDocumentReplacementSession **outSession)
{
    size_t needle_bytes = 0U, replacement_bytes = 0U;
    if (outSession == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSession = NULL;
    UmiEditorSearchOptions options;
    UmiStatus policy = DocumentSearchOptions(searchOptions, &options);
    if (policy != UMI_STATUS_OK) return policy;
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ReplacementSessionInput(needle, 0, &needle_bytes);
    if (status == UMI_STATUS_OK) status = ReplacementSessionInput(replacement, 1, &replacement_bytes);
    if (status != UMI_STATUS_OK) return status;
    size_t count = umi_document_coordinator_count(coordinator);
    if (count > UMI_DOCUMENT_MAX_WORKING_COPIES) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentReplacementSession *session = calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->coordinator = coordinator;
    session->searchOptions = options;
    session->needle = malloc(needle_bytes + 1U);
    session->replacement = malloc(replacement_bytes + 1U);
    if (count != 0U) session->targets = calloc(count, sizeof(*session->targets));
    if (session->needle == NULL || session->replacement == NULL || (count != 0U && session->targets == NULL)) {
        UmiDocumentReplacementSessionDestroy(session); return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(session->needle, needle, needle_bytes + 1U);
    memcpy(session->replacement, replacement, replacement_bytes + 1U);
    for (size_t index = 0U; index < count; ++index) {
        UmiDocumentWorkingCopySnapshot snapshot;
        status = umi_document_coordinator_at(coordinator, index, &snapshot);
        if (status != UMI_STATUS_OK) { UmiDocumentReplacementSessionDestroy(session); return status; }
        ReplacementTarget *target = &session->targets[index];
        target->id = snapshot.document_id;
        target->has_path = snapshot.has_path;
        memcpy(target->view_id, snapshot.view_id, sizeof(target->view_id));
        memcpy(target->path, snapshot.path, sizeof(target->path));
    }
    session->progress.total = count;
    session->progress.remaining = count;
    session->progress.phase = count == 0U ? UMI_DOCUMENT_REPLACEMENT_COMPLETE : UMI_DOCUMENT_REPLACEMENT_READY;
    session->progress.last_status = UMI_STATUS_OK;
    *outSession = session;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentReplacementSessionCreate(UmiDocumentCoordinator *coordinator, const char *needle,
    const char *replacement, UmiDocumentReplacementSession **outSession)
{
    return UmiDocumentReplacementSessionCreateWithOptions(coordinator, needle, replacement, NULL, outSession);
}


UmiStatus UmiDocumentReplacementSessionProgress(const UmiDocumentReplacementSession *session,
    UmiDocumentReplacementProgress *outProgress)
{
    if (session == NULL || outProgress == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outProgress = session->progress;
    return UMI_STATUS_OK;
}

/* Release only the current question. Every counted outcome advances exactly
 * one captured ID; the original identity list never changes with tab order. */
static UmiStatus ReplacementSessionAdvance(UmiDocumentReplacementSession *session)
{
    UmiDocumentReplacementPlanDestroy(session->plan);
    session->plan = NULL;
    ++session->next;
    session->progress.remaining = session->progress.total - session->next;
    session->progress.phase = session->progress.remaining == 0U
        ? UMI_DOCUMENT_REPLACEMENT_COMPLETE : UMI_DOCUMENT_REPLACEMENT_READY;
    session->progress.last_status = UMI_STATUS_OK;
    return UMI_STATUS_OK;
}

static UmiStatus ReplacementSessionFail(UmiDocumentReplacementSession *session, UmiStatus status)
{
    UmiDocumentReplacementPlanDestroy(session->plan);
    session->plan = NULL;
    session->progress.phase = UMI_DOCUMENT_REPLACEMENT_FAILED;
    session->progress.last_status = status;
    return status;
}

/* Each sequential review now uses the policy captured when its session was created, independent of later UI changes.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiDocumentReplacementSessionStep(UmiDocumentReplacementSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (ReplacementSessionTerminal(session->progress.phase)) return session->progress.last_status;
    if (session->progress.phase == UMI_DOCUMENT_REPLACEMENT_REVIEW) return UMI_STATUS_OK;
    const ReplacementTarget *target = &session->targets[session->next];
    memset(&session->progress.current, 0, sizeof(session->progress.current));
    session->progress.current.document_id = target->id;
    /* Resolve the stable document identity because earlier closes compact the
     * coordinator array. A Save As or replaced view must not redirect a review. */
    int found = 0;
    for (size_t index = 0U; index < umi_document_coordinator_count(session->coordinator); ++index) {
        UmiDocumentWorkingCopySnapshot snapshot;
        UmiStatus status = umi_document_coordinator_at(session->coordinator, index, &snapshot);
        if (status != UMI_STATUS_OK) return ReplacementSessionFail(session, status);
        if (snapshot.document_id != target->id) continue;
        if (snapshot.has_path != target->has_path || strcmp(snapshot.path, target->path) != 0 ||
            strcmp(snapshot.view_id, target->view_id) != 0)
            return ReplacementSessionFail(session, UMI_STATUS_INVALID_STATE);
        found = 1; break;
    }
    if (!found) { ++session->progress.closed; return ReplacementSessionAdvance(session); }
    UmiStatus status = UmiDocumentCoordinatorPrepareReplacement(session->coordinator,
        target->id, session->needle, session->replacement, &session->plan);
    if (status == UMI_STATUS_PERMISSION_DENIED) {
        ++session->progress.read_only;
        return ReplacementSessionAdvance(session);
    }
    if (status == UMI_STATUS_OK)
        status = UmiDocumentReplacementPlanSummary(session->plan, &session->progress.current);
    if (status != UMI_STATUS_OK) return ReplacementSessionFail(session, status);
    if (!session->progress.current.text_changes) {
        ++session->progress.unchanged;
        return ReplacementSessionAdvance(session);
    }
    session->progress.phase = UMI_DOCUMENT_REPLACEMENT_REVIEW;
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiDocumentReplacementSessionStep(UmiDocumentReplacementSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (ReplacementSessionTerminal(session->progress.phase)) return session->progress.last_status;
    if (session->progress.phase == UMI_DOCUMENT_REPLACEMENT_REVIEW) return UMI_STATUS_OK;
    const ReplacementTarget *target = &session->targets[session->next];
    memset(&session->progress.current, 0, sizeof(session->progress.current));
    session->progress.current.document_id = target->id;
    /* Resolve the stable document identity because earlier closes compact the
     * coordinator array. A Save As or replaced view must not redirect a review. */
    int found = 0;
    for (size_t index = 0U; index < umi_document_coordinator_count(session->coordinator); ++index) {
        UmiDocumentWorkingCopySnapshot snapshot;
        UmiStatus status = umi_document_coordinator_at(session->coordinator, index, &snapshot);
        if (status != UMI_STATUS_OK) return ReplacementSessionFail(session, status);
        if (snapshot.document_id != target->id) continue;
        if (snapshot.has_path != target->has_path || strcmp(snapshot.path, target->path) != 0 ||
            strcmp(snapshot.view_id, target->view_id) != 0)
            return ReplacementSessionFail(session, UMI_STATUS_INVALID_STATE);
        found = 1; break;
    }
    if (!found) { ++session->progress.closed; return ReplacementSessionAdvance(session); }
    UmiStatus status = UmiDocumentCoordinatorPrepareReplacementWithOptions(session->coordinator,
        target->id, session->needle, session->replacement, &session->searchOptions, &session->plan);
    if (status == UMI_STATUS_PERMISSION_DENIED) {
        ++session->progress.read_only;
        return ReplacementSessionAdvance(session);
    }
    if (status == UMI_STATUS_OK)
        status = UmiDocumentReplacementPlanSummary(session->plan, &session->progress.current);
    if (status != UMI_STATUS_OK) return ReplacementSessionFail(session, status);
    if (!session->progress.current.text_changes) {
        ++session->progress.unchanged;
        return ReplacementSessionAdvance(session);
    }
    session->progress.phase = UMI_DOCUMENT_REPLACEMENT_REVIEW;
    return UMI_STATUS_OK;
}

UmiStatus UmiDocumentReplacementSessionTexts(const UmiDocumentReplacementSession *session,
    const char **outPrevious, size_t *outPreviousBytes,
    const char **outProposed, size_t *outProposedBytes)
{
    if (outPrevious != NULL) *outPrevious = NULL;
    if (outPreviousBytes != NULL) *outPreviousBytes = 0U;
    if (outProposed != NULL) *outProposed = NULL;
    if (outProposedBytes != NULL) *outProposedBytes = 0U;
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->progress.phase != UMI_DOCUMENT_REPLACEMENT_REVIEW) return UMI_STATUS_INVALID_STATE;
    return UmiDocumentReplacementPlanTexts(session->plan, outPrevious, outPreviousBytes, outProposed, outProposedBytes);
}

UmiStatus UmiDocumentReplacementSessionCheck(const UmiDocumentReplacementSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->progress.phase != UMI_DOCUMENT_REPLACEMENT_REVIEW) return UMI_STATUS_INVALID_STATE;
    return UmiDocumentCoordinatorCheckReplacement(session->coordinator, session->plan);
}

UmiStatus UmiDocumentReplacementSessionRespond(UmiDocumentReplacementSession *session,
    UmiDocumentReplacementDecision decision)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (decision != UMI_DOCUMENT_REPLACEMENT_APPLY && decision != UMI_DOCUMENT_REPLACEMENT_SKIP &&
        decision != UMI_DOCUMENT_REPLACEMENT_STOP) return UMI_STATUS_INVALID_ARGUMENT;
    if (decision == UMI_DOCUMENT_REPLACEMENT_STOP) return UmiDocumentReplacementSessionCancel(session);
    if (session->progress.phase != UMI_DOCUMENT_REPLACEMENT_REVIEW) return UMI_STATUS_INVALID_STATE;
    if (decision == UMI_DOCUMENT_REPLACEMENT_SKIP) {
        ++session->progress.skipped; return ReplacementSessionAdvance(session);
    }
    /* Reserve the count before committing: even narrow size_t platforms must
     * not report success with a wrapped total after an otherwise valid edit. */
    if (session->progress.current.match_count > SIZE_MAX - session->progress.replacements)
        return ReplacementSessionFail(session, UMI_STATUS_CAPACITY_EXCEEDED);
    UmiStatus status = UmiDocumentCoordinatorApplyReplacement(session->coordinator, session->plan, NULL);
    if (status != UMI_STATUS_OK) return ReplacementSessionFail(session, status);
    session->progress.replacements += session->progress.current.match_count;
    ++session->progress.applied;
    return ReplacementSessionAdvance(session);
}

UmiStatus UmiDocumentReplacementSessionCancel(UmiDocumentReplacementSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!ReplacementSessionTerminal(session->progress.phase)) {
        UmiDocumentReplacementPlanDestroy(session->plan);
        session->plan = NULL;
        session->progress.phase = UMI_DOCUMENT_REPLACEMENT_CANCELLED;
        session->progress.last_status = UMI_STATUS_CANCELLED;
    }
    return UMI_STATUS_OK;
}
