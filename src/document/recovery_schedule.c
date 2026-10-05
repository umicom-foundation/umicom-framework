/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/recovery_schedule.c
 * PURPOSE: Track successful recovery acknowledgements separately from newer visible source revisions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/recovery_schedule.h"
#include <stdlib.h>
#include <string.h>
typedef struct RecoveryScheduledDocument
{
    UmiDocumentRecoveryObservation current;
    uint64_t saved_text_revision, saved_store_revision, due_ms;
} RecoveryScheduledDocument;
struct UmiDocumentRecoverySchedule
{
    RecoveryScheduledDocument documents[UMI_DOCUMENT_MAX_WORKING_COPIES];
    UmiDocumentRecoveryTicket pending;
    size_t count, cursor;
    uint64_t interval_ms, now_ms, next_sequence, saved_snapshots;
    UmiStatus last_status;
    int enabled, paused;
};
static int ScheduleObservationValid(const UmiDocumentRecoveryObservation *item)
{
    return item->document_id != 0U && item->text_revision != 0U && item->store_revision != 0U &&
           (item->dirty == 0 || item->dirty == 1);
}
static int ScheduleObservationEqual(const UmiDocumentRecoveryObservation *left,
                                    const UmiDocumentRecoveryObservation *right)
{
    return left->document_id == right->document_id && left->text_revision == right->text_revision &&
           left->store_revision == right->store_revision && left->dirty == right->dirty;
}
static size_t ScheduleFind(const UmiDocumentRecoverySchedule *schedule, UmiDocumentId id)
{
    for (size_t i = 0U; i < schedule->count; ++i)
        if (schedule->documents[i].current.document_id == id)
            return i;
    return SIZE_MAX;
}
static UmiStatus ScheduleTime(const UmiDocumentRecoverySchedule *schedule, uint64_t now, uint64_t interval)
{
    if (now < schedule->now_ms)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (now > UINT64_MAX - interval)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentRecoveryScheduleCreate(UmiDocumentRecoverySchedule **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = calloc(1U, sizeof(**out));
    if (*out == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    (*out)->interval_ms = 60000U;
    (*out)->last_status = UMI_STATUS_OK;
    return UMI_STATUS_OK;
}
void UmiDocumentRecoveryScheduleDestroy(UmiDocumentRecoverySchedule *schedule) { free(schedule); }
UmiStatus UmiDocumentRecoveryScheduleConfigure(UmiDocumentRecoverySchedule *schedule, int enabled,
                                               uint64_t interval, uint64_t now)
{
    if (schedule == NULL || (enabled != 0 && enabled != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (interval < UMI_DOCUMENT_RECOVERY_INTERVAL_MINIMUM_MS ||
        interval > UMI_DOCUMENT_RECOVERY_INTERVAL_MAXIMUM_MS)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ScheduleTime(schedule, now, interval);
    if (status != UMI_STATUS_OK)
        return status;
    schedule->interval_ms = interval;
    schedule->enabled = enabled;
    schedule->paused = 0;
    schedule->last_status = UMI_STATUS_OK;
    schedule->now_ms = now;
    for (size_t i = 0U; i < schedule->count; ++i)
        schedule->documents[i].due_ms = now + interval;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentRecoveryScheduleObserve(UmiDocumentRecoverySchedule *schedule,
                                             const UmiDocumentRecoveryObservation *documents, size_t count,
                                             uint64_t now)
{
    if (schedule == NULL || (count != 0U && documents == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_DOCUMENT_MAX_WORKING_COPIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = ScheduleTime(schedule, now, schedule->interval_ms);
    if (status != UMI_STATUS_OK)
        return status;
    for (size_t i = 0U; i < count; ++i)
    {
        if (!ScheduleObservationValid(&documents[i]))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j)
            if (documents[j].document_id == documents[i].document_id)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Build the complete candidate before replacing tracked identities. A late
     * duplicate or capacity failure cannot forget an earlier saved checkpoint. */
    RecoveryScheduledDocument candidate[UMI_DOCUMENT_MAX_WORKING_COPIES] = {0};
    UmiDocumentId next = schedule->count != 0U
                             ? schedule->documents[schedule->cursor % schedule->count].current.document_id
                             : 0U;
    size_t cursor = 0U;
    for (size_t i = 0U; i < count; ++i)
    {
        size_t previous = ScheduleFind(schedule, documents[i].document_id);
        if (previous != SIZE_MAX)
            candidate[i] = schedule->documents[previous];
        else
            candidate[i].due_ms = now + schedule->interval_ms;
        candidate[i].current = documents[i];
        if (documents[i].document_id == next)
            cursor = i;
    }
    memcpy(schedule->documents, candidate, sizeof(candidate));
    schedule->count = count;
    schedule->cursor = cursor;
    schedule->now_ms = now;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentRecoveryScheduleBegin(UmiDocumentRecoverySchedule *schedule, uint64_t now,
                                           UmiDocumentRecoveryTicket *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (schedule == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ScheduleTime(schedule, now, schedule->interval_ms);
    if (status != UMI_STATUS_OK)
        return status;
    if (schedule->pending.sequence != 0U)
        return UMI_STATUS_BUSY;
    if (!schedule->enabled || schedule->paused)
        return UMI_STATUS_UNAVAILABLE;
    if (schedule->next_sequence == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    schedule->now_ms = now;
    for (size_t offset = 0U; offset < schedule->count; ++offset)
    {
        size_t index = (schedule->cursor + offset) % schedule->count;
        RecoveryScheduledDocument *item = &schedule->documents[index];
        if (!item->current.dirty || now < item->due_ms ||
            (item->saved_text_revision == item->current.text_revision &&
             item->saved_store_revision == item->current.store_revision))
            continue;
        schedule->pending.owner = schedule;
        schedule->pending.sequence = ++schedule->next_sequence;
        schedule->pending.captured = item->current;
        schedule->cursor = (index + 1U) % schedule->count;
        *out = schedule->pending;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus UmiDocumentRecoveryScheduleFinish(UmiDocumentRecoverySchedule *schedule,
                                            const UmiDocumentRecoveryTicket *ticket, UmiStatus result,
                                            uint64_t now)
{
    if (schedule == NULL || ticket == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (ticket->owner != schedule || ticket->sequence == 0U ||
        ticket->sequence != schedule->pending.sequence ||
        !ScheduleObservationEqual(&ticket->captured, &schedule->pending.captured))
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status = ScheduleTime(schedule, now, schedule->interval_ms);
    if (status != UMI_STATUS_OK)
        return status;
    size_t index = ScheduleFind(schedule, ticket->captured.document_id);
    if (index != SIZE_MAX)
    {
        RecoveryScheduledDocument *item = &schedule->documents[index];
        item->due_ms = now + schedule->interval_ms;
        /* Completion acknowledges captured source only. Never copy the newest
         * observation into saved state while an older write is completing. */
        if (result == UMI_STATUS_OK)
        {
            item->saved_text_revision = ticket->captured.text_revision;
            item->saved_store_revision = ticket->captured.store_revision;
        }
    }
    if (result == UMI_STATUS_OK)
    {
        if (schedule->saved_snapshots != UINT64_MAX)
            ++schedule->saved_snapshots;
    }
    else
        schedule->paused = 1;
    schedule->last_status = result;
    schedule->now_ms = now;
    memset(&schedule->pending, 0, sizeof(schedule->pending));
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentRecoveryScheduleInspect(const UmiDocumentRecoverySchedule *schedule,
                                             UmiDocumentRecoveryScheduleInfo *out)
{
    if (schedule == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDocumentRecoveryScheduleInfo info = {0};
    info.documents = schedule->count;
    info.interval_ms = schedule->interval_ms;
    info.saved_snapshots = schedule->saved_snapshots;
    info.enabled = schedule->enabled;
    info.pending = schedule->pending.sequence != 0U;
    info.paused_after_failure = schedule->paused;
    info.last_status = schedule->last_status;
    *out = info;
    return UMI_STATUS_OK;
}

UmiStatus UmiDocumentRecoveryScheduleSuspend(UmiDocumentRecoverySchedule *schedule, UmiStatus reason,
                                             uint64_t now)
{
    if (schedule == NULL || reason == UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (schedule->pending.sequence != 0U)
        return UMI_STATUS_BUSY;
    UmiStatus status = ScheduleTime(schedule, now, schedule->interval_ms);
    if (status != UMI_STATUS_OK)
        return status;
    schedule->paused = 1;
    schedule->last_status = reason;
    schedule->now_ms = now;
    return UMI_STATUS_OK;
}
