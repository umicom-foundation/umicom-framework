/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_progress.c
 *
 * PURPOSE:
 *   Publish local checkpoint history only after a checked storage transaction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_progress_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus Begin(UmiLearningProgress *progress)
{
    if (progress->recovery_required) return UMI_STATUS_INVALID_STATE;
    if (umi_data_server_in_transaction(progress->server)) return UMI_STATUS_BUSY;
    return umi_data_server_begin(progress->server);
}
UmiStatus UmiLearningProgressRefresh(UmiLearningProgress *progress)
{
    if (progress == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    LpState *candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = Begin(progress);
    if (status == UMI_STATUS_OK) {
        status = LpLoad(progress, candidate);
        /* Even read transactions have an owner and can fail to release it.
         * Publish only after release succeeds; the old projection stays usable
         * for inspection if storage becomes uncertain. */
        status = LpRollback(progress, status);
        if (status == UMI_STATUS_OK) {
            free(progress->state);
            progress->state = candidate;
            candidate = NULL;
        }
    }
    free(candidate);
    return status;
}
UmiStatus UmiLearningProgressOpen(UmiDataServer *server, const char *learner_id,
    UmiLearningProgress **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = LpToken(learner_id);
    if (status != UMI_STATUS_OK) return status;
    UmiLearningProgress *progress = calloc(1U, sizeof(*progress));
    if (progress == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    progress->server = server;
    memcpy(progress->learner_id, learner_id, strlen(learner_id) + 1U);
    int n = snprintf(progress->prefix, sizeof(progress->prefix), "learning.progress/%s/", learner_id);
    if (n < 0 || (size_t)n >= sizeof(progress->prefix)) status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) status = UmiLearningProgressRefresh(progress);
    if (status != UMI_STATUS_OK) { UmiLearningProgressClose(progress); return status; }
    *out = progress;
    return UMI_STATUS_OK;
}
void UmiLearningProgressClose(UmiLearningProgress *progress)
{
    if (progress == NULL) return;
    free(progress->state);
    free(progress);
}
UmiStatus UmiLearningProgressCopySnapshot(const UmiLearningProgress *progress,
    UmiLearningProgressSnapshot *out)
{
    if (progress == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningProgressSnapshot snapshot = {0};
    memcpy(snapshot.learner_id, progress->learner_id, sizeof(snapshot.learner_id));
    snapshot.event_count = progress->state->count;
    snapshot.backend = umi_data_server_backend(progress->server);
    snapshot.recovery_required = progress->recovery_required;
    *out = snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningProgressEventAt(const UmiLearningProgress *progress, size_t index,
    UmiLearningProgressEvent *out)
{
    if (progress == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= progress->state->count) return UMI_STATUS_NOT_FOUND;
    *out = progress->state->events[index];
    return UMI_STATUS_OK;
}
static UmiStatus Capture(const UmiLearningReader *reader, const char *checkpoint,
    UmiLearningProgressEvent *out)
{
    UmiStatus status = LpToken(checkpoint);
    if (status != UMI_STATUS_OK) return status;
    UmiLearningReaderSnapshot selected;
    status = UmiLearningReaderCopySnapshot(reader, &selected);
    if (status != UMI_STATUS_OK) return status;
    if (!selected.has_document || (selected.selection.resource.kind != UMI_LEARNING_RESOURCE_GUIDE &&
        selected.selection.resource.kind != UMI_LEARNING_RESOURCE_LESSON)) return UMI_STATUS_INVALID_STATE;
    UmiLearningProgressEvent event = {0};
    memcpy(event.pack_id, selected.pack.id, sizeof(event.pack_id));
    event.pack_revision = selected.pack.revision;
    memcpy(event.resource_id, selected.selection.resource.id, sizeof(event.resource_id));
    memcpy(event.sha256, selected.selection.resource.sha256, sizeof(event.sha256));
    memcpy(event.checkpoint, checkpoint, strlen(checkpoint) + 1U);
    *out = event;
    return UMI_STATUS_OK;
}
static int SameCheckpoint(const UmiLearningProgressEvent *left, const UmiLearningProgressEvent *right)
{
    return left->pack_revision == right->pack_revision &&
        strcmp(left->pack_id, right->pack_id) == 0 &&
        strcmp(left->resource_id, right->resource_id) == 0 &&
        strcmp(left->sha256, right->sha256) == 0 &&
        strcmp(left->checkpoint, right->checkpoint) == 0;
}
UmiStatus UmiLearningProgressFindCurrent(const UmiLearningProgress *progress,
    const UmiLearningReader *reader, const char *checkpoint, UmiLearningProgressEvent *out)
{
    if (progress == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (progress->recovery_required) return UMI_STATUS_INVALID_STATE;
    UmiLearningProgressEvent selected;
    UmiStatus status = Capture(reader, checkpoint, &selected);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = progress->state->count; i > 0U; --i) {
        if (SameCheckpoint(&selected, &progress->state->events[i - 1U])) {
            *out = progress->state->events[i - 1U];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
/* Share the event/header commit boundary with archive imports. The original
 * single-event writer is retained for review; stale-state and output rules
 * are unchanged in the active implementation below. */
#if 0
UmiStatus UmiLearningProgressRecord(UmiLearningProgress *progress,
    const UmiLearningReader *reader, const char *checkpoint, const char *evidence_reference,
    UmiLearningProgressEvent *out)
{
    if (progress == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (progress->recovery_required) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = LpToken(evidence_reference);
    if (status != UMI_STATUS_OK) return status;
    UmiLearningProgressEvent event;
    status = Capture(reader, checkpoint, &event);
    if (status != UMI_STATUS_OK) return status;
    memcpy(event.evidence_reference, evidence_reference, strlen(evidence_reference) + 1U);
    if (umi_data_server_is_readonly(progress->server)) return UMI_STATUS_PERMISSION_DENIED;
    LpState *fresh = calloc(1U, sizeof(*fresh));
    if (fresh == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = Begin(progress);
    if (status != UMI_STATUS_OK) { free(fresh); return status; }
    status = LpLoad(progress, fresh);
    if (status == UMI_STATUS_OK && fresh->count != progress->state->count) status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK) {
        /* Compare canonical records, not structure padding or just a counter.
         * Same-count edits to historical evidence cannot be silently accepted. */
        for (size_t i = 0U; i < fresh->count; ++i) {
            if (strcmp(fresh->wire[i], progress->state->wire[i]) != 0) {
                status = UMI_STATUS_BUSY;
                break;
            }
        }
    }
    char wire[LP_WIRE_CAPACITY];
    if (status == UMI_STATUS_OK) status = LpEncode(&event, wire);
    if (status == UMI_STATUS_OK) {
        for (size_t i = 0U; i < fresh->count; ++i)
            if (strcmp(fresh->wire[i], wire) == 0) { status = UMI_STATUS_ALREADY_EXISTS; break; }
    }
    if (status == UMI_STATUS_OK && fresh->count == UMI_LEARNING_PROGRESS_MAX_EVENTS)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) {
        char suffix[32], key[LP_KEY_CAPACITY], head[32];
        int n = snprintf(suffix, sizeof(suffix), "event/%zu", fresh->count + 1U);
        if (n < 0 || (size_t)n >= sizeof(suffix)) status = UMI_STATUS_INTERNAL_ERROR;
        if (status == UMI_STATUS_OK) status = LpKey(progress, suffix, key);
        if (status == UMI_STATUS_OK) status = umi_data_server_set(progress->server, key, wire);
        n = snprintf(head, sizeof(head), "LP1|%zu", fresh->count + 1U);
        if (n < 0 || (size_t)n >= sizeof(head)) status = UMI_STATUS_INTERNAL_ERROR;
        if (status == UMI_STATUS_OK) status = LpKey(progress, "head", key);
        if (status == UMI_STATUS_OK) status = umi_data_server_set(progress->server, key, head);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_data_server_commit(progress->server);
        if (status != UMI_STATUS_OK) {
            progress->recovery_required = 1;
            if (umi_data_server_in_transaction(progress->server))
                (void)LpRollback(progress, status);
        } else {
            event.sequence = (uint64_t)fresh->count + 1U;
            fresh->events[fresh->count] = event;
            memcpy(fresh->wire[fresh->count], wire, strlen(wire) + 1U);
            ++fresh->count;
            free(progress->state);
            progress->state = fresh;
            fresh = NULL;
            *out = event;
        }
    } else {
        status = LpRollback(progress, status);
    }
    free(fresh);
    return status;
}
#endif
UmiStatus UmiLearningProgressRecord(UmiLearningProgress *progress,
    const UmiLearningReader *reader, const char *checkpoint, const char *evidence_reference,
    UmiLearningProgressEvent *out)
{
    if (progress == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (progress->recovery_required) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = LpToken(evidence_reference);
    if (status != UMI_STATUS_OK) return status;
    UmiLearningProgressEvent event;
    status = Capture(reader, checkpoint, &event);
    if (status != UMI_STATUS_OK) return status;
    memcpy(event.evidence_reference, evidence_reference, strlen(evidence_reference) + 1U);
    if (umi_data_server_is_readonly(progress->server)) return UMI_STATUS_PERMISSION_DENIED;
    LpState *fresh = calloc(1U, sizeof(*fresh));
    if (fresh == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = Begin(progress);
    if (status != UMI_STATUS_OK) { free(fresh); return status; }
    status = LpLoad(progress, fresh);
    if (status == UMI_STATUS_OK && fresh->count != progress->state->count) status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK) {
        /* Compare canonical records, not structure padding or just a counter.
         * Same-count edits to historical evidence cannot be silently accepted. */
        for (size_t i = 0U; i < fresh->count; ++i) {
            if (strcmp(fresh->wire[i], progress->state->wire[i]) != 0) {
                status = UMI_STATUS_BUSY;
                break;
            }
        }
    }
    char wire[LP_WIRE_CAPACITY];
    if (status == UMI_STATUS_OK) status = LpEncode(&event, wire);
    if (status == UMI_STATUS_OK) {
        for (size_t i = 0U; i < fresh->count; ++i)
            if (strcmp(fresh->wire[i], wire) == 0) { status = UMI_STATUS_ALREADY_EXISTS; break; }
    }
    if (status == UMI_STATUS_OK && fresh->count == UMI_LEARNING_PROGRESS_MAX_EVENTS)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) {
        size_t first = fresh->count;
        event.sequence = (uint64_t)first + 1U;
        fresh->events[first] = event;
        memcpy(fresh->wire[first], wire, strlen(wire) + 1U);
        ++fresh->count;
        status = LpStore(progress, fresh, first);
        if (status == UMI_STATUS_OK) {
            free(progress->state);
            progress->state = fresh;
            fresh = NULL;
            *out = event;
        }
    } else {
        status = LpRollback(progress, status);
    }
    free(fresh);
    return status;
}
