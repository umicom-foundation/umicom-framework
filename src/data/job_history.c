/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/job_history.c
 * PURPOSE: Commit shared job evidence atomically and refuse stale updates.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "job_history_internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiJobHistory
{
    UmiDataServer *server;
    char prefix[96];
};
typedef struct JobRecords
{
    UmiJobHistoryEntry entries[UMI_JOB_HISTORY_CAPACITY];
    uint64_t next_id;
} JobRecords;

bool UmiJobHistoryIsFinished(UmiJobHistoryState state)
{
    return state == UMI_JOB_HISTORY_SUCCEEDED || state == UMI_JOB_HISTORY_FAILED ||
           state == UMI_JOB_HISTORY_CANCELLED;
}
const char *UmiJobHistoryStateText(UmiJobHistoryState state)
{
    switch (state)
    {
    case UMI_JOB_HISTORY_PREPARED:
        return "Prepared; outcome unknown outside this session";
    case UMI_JOB_HISTORY_RUNNING:
        return "Started; outcome unknown outside this session";
    case UMI_JOB_HISTORY_SUCCEEDED:
        return "Succeeded";
    case UMI_JOB_HISTORY_FAILED:
        return "Failed";
    case UMI_JOB_HISTORY_CANCELLED:
        return "Cancelled";
    default:
        return "Invalid job state";
    }
}
UmiStatus UmiJobHistoryCreate(UmiDataServer *server, const char *scope, UmiJobHistory **out_history)
{
    if (out_history == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;
    if (server == NULL || !UmiJobHistoryIdentifier(scope, 65U))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiJobHistory *history = calloc(1U, sizeof(*history));
    if (history == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    history->server = server;
    (void)snprintf(history->prefix, sizeof(history->prefix), "job-history/%s/", scope);
    *out_history = history;
    return UMI_STATUS_OK;
}
void UmiJobHistoryDestroy(UmiJobHistory *history) { free(history); }

static void slot_key(const UmiJobHistory *history, size_t slot, char *key, size_t capacity)
{
    (void)snprintf(key, capacity, "%sslot/%zu", history->prefix, slot);
}
/* Every operation reserves its own transaction. A failed begin never permits
 * rollback, so another caller's transaction cannot accidentally be cancelled. */
static UmiStatus finish(UmiJobHistory *history, UmiStatus status)
{
    if (status == UMI_STATUS_OK)
        status = umi_data_server_commit(history->server);
    if (status != UMI_STATUS_OK)
    {
        UmiStatus rollback = umi_data_server_rollback(history->server);
        if (rollback != UMI_STATUS_OK)
            return rollback;
    }
    return status;
}
/* Read all bounded slots to detect corrupt metadata, duplicate identifiers and
 * unsupported records before making any change. A missing slot is not a job. */
static UmiStatus load(UmiJobHistory *history, JobRecords *records)
{
    char key[128], wire[UMI_JOB_HISTORY_WIRE_CAPACITY];
    (void)snprintf(key, sizeof(key), "%snext", history->prefix);
    UmiStatus status = umi_data_server_get(history->server, key, wire, sizeof(wire));
    bool fresh = status == UMI_STATUS_NOT_FOUND;
    if (fresh)
        records->next_id = 1U;
    else if (status != UMI_STATUS_OK)
        return status;
    else
    {
        const char *cursor = wire;
        uint64_t format = 0;
        if (!UmiJobHistoryReadNumber(&cursor, &format, '|') || format != 1U ||
            !UmiJobHistoryReadNumber(&cursor, &records->next_id, '\0') || records->next_id == 0)
            return UMI_STATUS_PARSE_ERROR;
    }
    for (size_t i = 0; i < UMI_JOB_HISTORY_CAPACITY; ++i)
    {
        slot_key(history, i, key, sizeof(key));
        status = umi_data_server_get(history->server, key, wire, sizeof(wire));
        if (status == UMI_STATUS_NOT_FOUND)
            continue;
        if (status != UMI_STATUS_OK)
            return status;
        status = UmiJobHistoryDecode(wire, &records->entries[i]);
        if (status != UMI_STATUS_OK)
            return status;
        if (fresh || records->entries[i].id >= records->next_id)
            return UMI_STATUS_PARSE_ERROR;
        for (size_t j = 0; j < i; ++j)
            if (records->entries[j].id == records->entries[i].id)
                return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
static UmiStatus store(UmiJobHistory *history, size_t slot, const UmiJobHistoryEntry *entry)
{
    char key[128], wire[UMI_JOB_HISTORY_WIRE_CAPACITY];
    UmiStatus status = UmiJobHistoryEncode(entry, wire, sizeof(wire));
    if (status != UMI_STATUS_OK)
        return status;
    slot_key(history, slot, key, sizeof(key));
    return umi_data_server_set(history->server, key, wire);
}
static UmiStatus start(UmiJobHistory *history, JobRecords **out_records)
{
    JobRecords *records = calloc(1U, sizeof(*records));
    if (records == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_data_server_begin(history->server);
    if (status != UMI_STATUS_OK)
    {
        free(records);
        return status;
    }
    status = load(history, records);
    if (status != UMI_STATUS_OK)
    {
        status = finish(history, status);
        free(records);
        return status;
    }
    *out_records = records;
    return UMI_STATUS_OK;
}
UmiStatus UmiJobHistoryBegin(UmiJobHistory *history, const char *kind, const char *label,
                             unsigned total_steps, UmiJobHistoryEntry *out_entry)
{
    if (history == NULL || out_entry == NULL ||
        !UmiJobHistoryIdentifier(kind, UMI_JOB_HISTORY_KIND_CAPACITY) ||
        !UmiJobHistoryCaption(label, UMI_JOB_HISTORY_LABEL_CAPACITY) || total_steps == 0 || total_steps > 64U)
        return UMI_STATUS_INVALID_ARGUMENT;
    JobRecords *records = NULL;
    UmiStatus status = start(history, &records);
    if (status != UMI_STATUS_OK)
        return status;
    size_t slot = 0;
    while (slot < UMI_JOB_HISTORY_CAPACITY && records->entries[slot].id != 0)
        ++slot;
    UmiJobHistoryEntry entry = {0};
    if (slot == UMI_JOB_HISTORY_CAPACITY || records->next_id == UINT64_MAX)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    else
    {
        entry.id = records->next_id;
        entry.revision = 1U;
        entry.state = UMI_JOB_HISTORY_PREPARED;
        entry.total_steps = total_steps;
        memcpy(entry.kind, kind, strlen(kind) + 1U);
        memcpy(entry.label, label, strlen(label) + 1U);
        status = store(history, slot, &entry);
        if (status == UMI_STATUS_OK)
        {
            char key[128], next[32];
            (void)snprintf(key, sizeof(key), "%snext", history->prefix);
            (void)snprintf(next, sizeof(next), "1|%" PRIu64, records->next_id + 1U);
            status = umi_data_server_set(history->server, key, next);
        }
    }
    status = finish(history, status);
    free(records);
    if (status == UMI_STATUS_OK)
        *out_entry = entry;
    return status;
}
UmiStatus UmiJobHistoryUpdate(UmiJobHistory *history, uint64_t id, uint64_t expected_revision,
                              UmiJobHistoryState state, unsigned completed_steps, UmiStatus result,
                              UmiJobHistoryEntry *out_entry)
{
    if (history == NULL || out_entry == NULL || id == 0 || expected_revision == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    JobRecords *records = NULL;
    UmiStatus status = start(history, &records);
    if (status != UMI_STATUS_OK)
        return status;
    size_t slot = 0;
    while (slot < UMI_JOB_HISTORY_CAPACITY && records->entries[slot].id != id)
        ++slot;
    UmiJobHistoryEntry entry = {0};
    if (slot == UMI_JOB_HISTORY_CAPACITY)
        status = UMI_STATUS_NOT_FOUND;
    else
    {
        entry = records->entries[slot];
        if (entry.revision != expected_revision)
            status = UMI_STATUS_BUSY;
        else if (UmiJobHistoryIsFinished(entry.state) || state == UMI_JOB_HISTORY_PREPARED ||
                 completed_steps < entry.completed_steps ||
                 (entry.state == UMI_JOB_HISTORY_PREPARED &&
                  (completed_steps != 0 || state == UMI_JOB_HISTORY_SUCCEEDED)))
            status = UMI_STATUS_INVALID_STATE;
        else if (entry.revision == UINT64_MAX)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
        {
            ++entry.revision;
            entry.state = state;
            entry.completed_steps = completed_steps;
            entry.result = result;
            status = store(history, slot, &entry);
        }
    }
    status = finish(history, status);
    free(records);
    if (status == UMI_STATUS_OK)
        *out_entry = entry;
    return status;
}
static int compare_entries(const void *left, const void *right)
{
    const UmiJobHistoryEntry *a = left, *b = right;
    return (a->id > b->id) - (a->id < b->id);
}
UmiStatus UmiJobHistoryCapture(UmiJobHistory *history, UmiJobHistorySnapshot *out_snapshot)
{
    if (history == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    JobRecords *records = NULL;
    UmiStatus status = start(history, &records);
    if (status != UMI_STATUS_OK)
        return status;
    status = finish(history, UMI_STATUS_OK);
    if (status == UMI_STATUS_OK)
    {
        memset(out_snapshot, 0, sizeof(*out_snapshot));
        for (size_t i = 0; i < UMI_JOB_HISTORY_CAPACITY; ++i)
            if (records->entries[i].id != 0)
            {
                out_snapshot->entries[out_snapshot->count++] = records->entries[i];
                if (!UmiJobHistoryIsFinished(records->entries[i].state))
                    ++out_snapshot->unfinished_count;
            }
        qsort(out_snapshot->entries, out_snapshot->count, sizeof(out_snapshot->entries[0]), compare_entries);
    }
    free(records);
    return status;
}
UmiStatus UmiJobHistoryPruneFinished(UmiJobHistory *history, size_t *out_removed)
{
    if (history == NULL || out_removed == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    JobRecords *records = NULL;
    UmiStatus status = start(history, &records);
    if (status != UMI_STATUS_OK)
        return status;
    size_t removed = 0;
    for (size_t i = 0; status == UMI_STATUS_OK && i < UMI_JOB_HISTORY_CAPACITY; ++i)
    {
        if (records->entries[i].id != 0 && UmiJobHistoryIsFinished(records->entries[i].state))
        {
            char key[128];
            slot_key(history, i, key, sizeof(key));
            status = umi_data_server_delete(history->server, key);
            if (status == UMI_STATUS_OK)
                ++removed;
        }
    }
    status = finish(history, status);
    free(records);
    if (status == UMI_STATUS_OK)
        *out_removed = removed;
    return status;
}
