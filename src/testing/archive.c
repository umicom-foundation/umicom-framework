/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/archive.c
 * PURPOSE: Store completed test evidence atomically without changing execution state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "archive_internal.h"
#include "selection_identity_internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiTestArchive
{
    UmiDataServer *server;
    char prefix[96];
};
typedef struct ArchiveRecords
{
    UmiTestArchiveEntry entries[UMI_TEST_ARCHIVE_CAPACITY];
    uint64_t next_id;
    char wire[UMI_TEST_ARCHIVE_WIRE_CAPACITY];
} ArchiveRecords;

/* Scope names form part of a key, never a filesystem path. Keep the accepted
 * alphabet deliberately small so one component cannot address another scope. */
static bool scope_valid(const char *scope)
{
    if (scope == NULL || scope[0] == '\0')
        return false;
    for (size_t i = 0; i < 65U; ++i)
    {
        unsigned char c = (unsigned char)scope[i];
        if (c == '\0')
            return true;
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.' ||
              c == '-' || c == '_'))
            return false;
    }
    return false;
}
UmiStatus UmiTestArchiveCreate(UmiDataServer *server, const char *scope, UmiTestArchive **out_archive)
{
    if (out_archive == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_archive = NULL;
    if (server == NULL || !scope_valid(scope))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestArchive *archive = calloc(1U, sizeof(*archive));
    if (archive == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    archive->server = server;
    (void)snprintf(archive->prefix, sizeof(archive->prefix), "ctest-archive/%s/", scope);
    *out_archive = archive;
    return UMI_STATUS_OK;
}
void UmiTestArchiveDestroy(UmiTestArchive *archive) { free(archive); }
static void slot_key(const UmiTestArchive *archive, size_t slot, char key[160])
{
    (void)snprintf(key, 160U, "%sslot/%zu", archive->prefix, slot);
}
static void row_key(const UmiTestArchive *archive, uint64_t id, const char *kind, size_t index, char key[160])
{
    (void)snprintf(key, 160U, "%srun/%" PRIu64 "/%s/%zu", archive->prefix, id, kind, index);
}
/* Finish only transactions acquired by start. A failed commit is followed by
 * rollback; a rollback error is reported instead of claiming that recovery
 * succeeded. No output object is published before this function succeeds. */
static UmiStatus finish(UmiTestArchive *archive, UmiStatus status)
{
    if (status == UMI_STATUS_OK)
        status = umi_data_server_commit(archive->server);
    if (status != UMI_STATUS_OK)
    {
        UmiStatus rollback = umi_data_server_rollback(archive->server);
        if (rollback != UMI_STATUS_OK)
            return rollback;
    }
    return status;
}
static bool parse_next(const char *wire, uint64_t *out_next)
{
    if (wire[0] != '1' || wire[1] != '|')
        return false;
    const char *p = wire + 2;
    uint64_t value = 0;
    if (*p < '0' || *p > '9')
        return false;
    while (*p >= '0' && *p <= '9')
    {
        unsigned digit = (unsigned)(*p - '0');
        if (value > (UINT64_MAX - digit) / 10U)
            return false;
        value = value * 10U + digit;
        ++p;
    }
    if (*p != '\0' || value == 0)
        return false;
    *out_next = value;
    return true;
}
static UmiStatus load(UmiTestArchive *archive, ArchiveRecords *records)
{
    char key[160];
    (void)snprintf(key, sizeof(key), "%snext", archive->prefix);
    UmiStatus status = umi_data_server_get(archive->server, key, records->wire, sizeof(records->wire));
    bool fresh = status == UMI_STATUS_NOT_FOUND;
    if (fresh)
        records->next_id = 1U;
    else if (status != UMI_STATUS_OK)
        return status;
    else if (!parse_next(records->wire, &records->next_id))
        return UMI_STATUS_PARSE_ERROR;
    /* Validate every bounded slot before reading or changing any entry. Missing
     * metadata with surviving slots is corruption, not permission to reuse IDs. */
    for (size_t i = 0; i < UMI_TEST_ARCHIVE_CAPACITY; ++i)
    {
        slot_key(archive, i, key);
        status = umi_data_server_get(archive->server, key, records->wire, sizeof(records->wire));
        if (status == UMI_STATUS_NOT_FOUND)
            continue;
        if (status != UMI_STATUS_OK)
            return status;
        status = UmiTestArchiveDecodeEntry(records->wire, &records->entries[i]);
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
static UmiStatus start(UmiTestArchive *archive, ArchiveRecords **out_records)
{
    ArchiveRecords *records = calloc(1U, sizeof(*records));
    if (records == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_data_server_begin(archive->server);
    if (status != UMI_STATUS_OK)
    {
        free(records);
        return status;
    }
    status = load(archive, records);
    if (status != UMI_STATUS_OK)
    {
        status = finish(archive, status);
        free(records);
        return status;
    }
    *out_records = records;
    return UMI_STATUS_OK;
}
static size_t find(const ArchiveRecords *records, uint64_t id)
{
    for (size_t i = 0; i < UMI_TEST_ARCHIVE_CAPACITY; ++i)
        if (records->entries[i].id == id)
            return i;
    return UMI_TEST_ARCHIVE_CAPACITY;
}
static UmiStatus request_read(UmiTestArchive *archive, ArchiveRecords *records,
                              const UmiTestArchiveEntry *entry, size_t index, UmiCtestJobRequest *request)
{
    if (index >= entry->plan.request_count)
        return UMI_STATUS_NOT_FOUND;
    char key[160];
    row_key(archive, entry->id, "request", index, key);
    UmiStatus status = umi_data_server_get(archive->server, key, records->wire, sizeof(records->wire));
    /* A referenced row must exist. Distinguish broken storage from an index
     * outside the published selection, which legitimately returns NOT_FOUND. */
    if (status == UMI_STATUS_NOT_FOUND)
        return UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK)
        return status;
    return UmiTestArchiveDecodeRequest(records->wire, request);
}
static UmiStatus result_read(UmiTestArchive *archive, ArchiveRecords *records,
                             const UmiTestArchiveEntry *entry, size_t index, UmiTestResult *result)
{
    if (index >= entry->run.completed)
        return UMI_STATUS_NOT_FOUND;
    UmiCtestJobRequest request = {0};
    UmiStatus status = request_read(archive, records, entry, index % entry->plan.request_count, &request);
    if (status != UMI_STATUS_OK)
        return status;
    char key[160];
    row_key(archive, entry->id, "result", index, key);
    status = UmiTestArchiveValueRead(archive->server, key, records->wire, sizeof(records->wire));
    if (status != UMI_STATUS_OK)
        return status;
    status = UmiTestArchiveDecodeResult(records->wire, result);
    if (status != UMI_STATUS_OK)
        return status;
    if (strcmp(result->test_id, request.test_id) != 0 || strcmp(result->name, request.name) != 0 ||
        (!entry->origin.retain_output && result->output[0] != '\0'))
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}
/* Recompute summary evidence from each copied result. Saturation matches the
 * execution service and avoids wrapping a long run's accumulated duration. */
static void accumulate(UmiCtestJobSnapshot *total, const UmiTestResult *result)
{
    ++total->completed;
    switch (result->state)
    {
    case UMI_TEST_STATE_PASSED:
        ++total->passed;
        break;
    case UMI_TEST_STATE_FAILED:
        ++total->failed;
        break;
    case UMI_TEST_STATE_SKIPPED:
        ++total->skipped;
        break;
    case UMI_TEST_STATE_CANCELLED:
        ++total->cancelled;
        break;
    case UMI_TEST_STATE_TIMED_OUT:
        ++total->timed_out;
        break;
    default:
        ++total->not_run;
        break;
    }
    total->duration_ms = UINT64_MAX - total->duration_ms < result->duration_ms
                             ? UINT64_MAX
                             : total->duration_ms + result->duration_ms;
    if (total->first_error == UMI_STATUS_OK && result->status != UMI_STATUS_OK)
        total->first_error = result->status;
}
static bool counts_equal(const UmiCtestJobSnapshot *left, const UmiCtestJobSnapshot *right)
{
    return left->completed == right->completed && left->passed == right->passed &&
           left->failed == right->failed && left->skipped == right->skipped &&
           left->cancelled == right->cancelled && left->timed_out == right->timed_out &&
           left->not_run == right->not_run && left->duration_ms == right->duration_ms &&
           left->first_error == right->first_error;
}
UmiStatus UmiTestArchiveSave(UmiTestArchive *archive, UmiCtestJob *source, const UmiTestArchiveOrigin *origin,
                             const UmiCancellationToken *cancellation, UmiTestArchiveEntry *out_entry)
{
    if (archive == NULL || source == NULL || out_entry == NULL || !UmiTestArchiveOriginValid(origin))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestArchiveEntry entry = {0};
    entry.id = 1U;
    entry.origin = *origin;
    UmiStatus status = UmiCtestJobGetSnapshot(source, &entry.run);
    if (status != UMI_STATUS_OK)
        return status;
    if (entry.run.state == UMI_TASK_CREATED || entry.run.state == UMI_TASK_QUEUED ||
        entry.run.state == UMI_TASK_RUNNING)
        return UMI_STATUS_BUSY;
    status = UmiCtestJobReadPlan(source, &entry.plan);
    if (status != UMI_STATUS_OK)
        return status;
    if (!UmiTestArchiveEntryValid(&entry))
        return UMI_STATUS_INVALID_STATE;
    if (umi_cancellation_token_is_requested(cancellation))
        return UMI_STATUS_CANCELLED;
    ArchiveRecords *records = NULL;
    status = start(archive, &records);
    if (status != UMI_STATUS_OK)
        return status;
    UmiTestResult *result = calloc(1U, sizeof(*result));
    size_t slot = find(records, 0);
    if (result == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    else if (slot == UMI_TEST_ARCHIVE_CAPACITY || records->next_id == UINT64_MAX)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    entry.id = records->next_id;
    char key[160];
    /* Hash the same copied requests that are written below. A transaction
     * publishes outcomes and selection evidence together, never in two stages. */
    UmiTestSelectionHasher selection;
    UmiTestSelectionBegin(&selection, &entry.plan);
    /* Requests preserve the entire intended selection, including disabled tests
     * and attempts that were never reached after cancellation or an early stop. */
    for (size_t i = 0; status == UMI_STATUS_OK && i < entry.plan.request_count; ++i)
    {
        UmiCtestJobRequest request = {0};
        if (umi_cancellation_token_is_requested(cancellation))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = UmiCtestJobRequestAt(source, i, &request);
        if (status == UMI_STATUS_OK)
            status = UmiTestArchiveEncodeRequest(&request, records->wire, sizeof(records->wire));
        if (status == UMI_STATUS_OK)
        {
            UmiTestSelectionAdd(&selection, &request);
            row_key(archive, entry.id, "request", i, key);
            status = umi_data_server_set(archive->server, key, records->wire);
        }
    }
    if (status == UMI_STATUS_OK)
        status = UmiTestSelectionFinish(&selection, entry.selection_digest);
    UmiCtestJobSnapshot total = {0};
    for (size_t i = 0; status == UMI_STATUS_OK && i < entry.run.completed; ++i)
    {
        uint32_t attempt = 0;
        UmiCtestJobRequest request = {0};
        if (umi_cancellation_token_is_requested(cancellation))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = UmiCtestJobResultAt(source, i, result, &attempt);
        if (status == UMI_STATUS_OK)
            status = request_read(archive, records, &entry, i % entry.plan.request_count, &request);
        if (status == UMI_STATUS_OK && !UmiTestArchiveResultValid(result))
            status = UMI_STATUS_INVALID_STATE;
        if (status == UMI_STATUS_OK &&
            (attempt != i / entry.plan.request_count + 1U || strcmp(result->test_id, request.test_id) != 0 ||
             strcmp(result->name, request.name) != 0))
            status = UMI_STATUS_INVALID_STATE;
        if (status != UMI_STATUS_OK)
            break;
        if (!entry.origin.retain_output)
            memset(result->output, 0, sizeof(result->output));
        status = UmiTestArchiveEncodeResult(result, records->wire, sizeof(records->wire));
        if (status == UMI_STATUS_OK)
        {
            accumulate(&total, result);
            row_key(archive, entry.id, "result", i, key);
            status = UmiTestArchiveValueWrite(archive->server, key, records->wire);
        }
    }
    if (status == UMI_STATUS_OK && !counts_equal(&entry.run, &total))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = UmiTestArchiveEncodeEntry(&entry, records->wire, sizeof(records->wire));
    if (status == UMI_STATUS_OK)
    {
        slot_key(archive, slot, key);
        status = umi_data_server_set(archive->server, key, records->wire);
    }
    if (status == UMI_STATUS_OK)
    {
        (void)snprintf(key, sizeof(key), "%snext", archive->prefix);
        (void)snprintf(records->wire, sizeof(records->wire), "1|%" PRIu64, entry.id + 1U);
        status = umi_data_server_set(archive->server, key, records->wire);
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancellation))
        status = UMI_STATUS_CANCELLED;
    status = finish(archive, status);
    free(result);
    free(records);
    if (status == UMI_STATUS_OK)
        *out_entry = entry;
    return status;
}
UmiStatus UmiTestArchiveList(UmiTestArchive *archive, UmiTestArchiveCatalog *output)
{
    if (archive == NULL || output == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ArchiveRecords *records = NULL;
    UmiStatus status = start(archive, &records);
    if (status != UMI_STATUS_OK)
        return status;
    UmiTestArchiveCatalog *catalog = calloc(1U, sizeof(*catalog));
    if (catalog == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    else
    {
        /* Insertion into a bounded catalogue keeps ordering stable when a slot
         * is reused after an explicit removal. Durable IDs never go backwards. */
        for (size_t i = 0; i < UMI_TEST_ARCHIVE_CAPACITY; ++i)
        {
            const UmiTestArchiveEntry *entry = &records->entries[i];
            if (entry->id == 0)
                continue;
            size_t j = catalog->count++;
            while (j > 0 && catalog->entries[j - 1U].id > entry->id)
            {
                catalog->entries[j] = catalog->entries[j - 1U];
                --j;
            }
            catalog->entries[j] = *entry;
        }
    }
    status = finish(archive, status);
    if (status == UMI_STATUS_OK)
        *output = *catalog;
    free(catalog);
    free(records);
    return status;
}
UmiStatus UmiTestArchiveRead(UmiTestArchive *archive, uint64_t id, UmiTestArchiveEntry *output)
{
    if (archive == NULL || id == 0 || output == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ArchiveRecords *records = NULL;
    UmiStatus status = start(archive, &records);
    if (status != UMI_STATUS_OK)
        return status;
    size_t slot = find(records, id);
    if (slot == UMI_TEST_ARCHIVE_CAPACITY)
        status = UMI_STATUS_NOT_FOUND;
    status = finish(archive, status);
    if (status == UMI_STATUS_OK)
        *output = records->entries[slot];
    free(records);
    return status;
}
UmiStatus UmiTestArchiveRequestAt(UmiTestArchive *archive, uint64_t id, size_t index,
                                  UmiCtestJobRequest *output)
{
    if (archive == NULL || id == 0 || output == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ArchiveRecords *records = NULL;
    UmiStatus status = start(archive, &records);
    if (status != UMI_STATUS_OK)
        return status;
    size_t slot = find(records, id);
    UmiCtestJobRequest request = {0};
    if (slot == UMI_TEST_ARCHIVE_CAPACITY)
        status = UMI_STATUS_NOT_FOUND;
    else
        status = request_read(archive, records, &records->entries[slot], index, &request);
    status = finish(archive, status);
    if (status == UMI_STATUS_OK)
        *output = request;
    free(records);
    return status;
}
UmiStatus UmiTestArchiveResultAt(UmiTestArchive *archive, uint64_t id, size_t index, UmiTestResult *output,
                                 uint32_t *out_attempt)
{
    if (archive == NULL || id == 0 || output == NULL || out_attempt == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ArchiveRecords *records = NULL;
    UmiStatus status = start(archive, &records);
    if (status != UMI_STATUS_OK)
        return status;
    UmiTestResult *result = calloc(1U, sizeof(*result));
    size_t slot = find(records, id);
    if (result == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    else if (slot == UMI_TEST_ARCHIVE_CAPACITY)
        status = UMI_STATUS_NOT_FOUND;
    else
        status = result_read(archive, records, &records->entries[slot], index, result);
    status = finish(archive, status);
    if (status == UMI_STATUS_OK)
    {
        *output = *result;
        *out_attempt = (uint32_t)(index / records->entries[slot].plan.request_count + 1U);
    }
    free(result);
    free(records);
    return status;
}
/* Keep metadata, request and outcome inside one transaction. Reading them in
 * separate UI calls could mix observations if another owner removed the run. */
UmiStatus UmiTestArchiveReadAttempt(UmiTestArchive *archive, uint64_t id, size_t index,
    const UmiCancellationToken *cancellation, UmiTestArchiveAttempt *out_attempt)
{
    if (archive == NULL || id == 0 || out_attempt == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancellation))
        return UMI_STATUS_CANCELLED;
    ArchiveRecords *records = NULL;
    UmiStatus status = start(archive, &records);
    if (status != UMI_STATUS_OK)
        return status;
    UmiTestArchiveAttempt *copy = calloc(1U, sizeof(*copy));
    size_t slot = find(records, id);
    if (copy == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    else if (slot == UMI_TEST_ARCHIVE_CAPACITY)
        status = UMI_STATUS_NOT_FOUND;
    else
    {
        copy->entry = records->entries[slot];
        status = result_read(archive, records, &copy->entry, index, &copy->result);
        if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancellation))
            status = UMI_STATUS_CANCELLED;
        if (status == UMI_STATUS_OK)
            status = request_read(archive, records, &copy->entry,
                index % copy->entry.plan.request_count, &copy->request);
        if (status == UMI_STATUS_OK)
            copy->attempt = (uint32_t)(index / copy->entry.plan.request_count + 1U);
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancellation))
        status = UMI_STATUS_CANCELLED;
    status = finish(archive, status);
    if (status == UMI_STATUS_OK)
        *out_attempt = *copy;
    free(copy);
    free(records);
    return status;
}
/* Interactive clients need cooperative cancellation between storage operations. The cancellable transaction below preserves all-or-nothing removal; the original implementation remains for engineering review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiTestArchiveRemove(UmiTestArchive *archive, uint64_t id)
{
    if (archive == NULL || id == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    ArchiveRecords *records = NULL;
    UmiStatus status = start(archive, &records);
    if (status != UMI_STATUS_OK)
        return status;
    size_t slot = find(records, id);
    if (slot == UMI_TEST_ARCHIVE_CAPACITY)
        status = UMI_STATUS_NOT_FOUND;
    char key[160];
    if (status == UMI_STATUS_OK)
    {
        const UmiTestArchiveEntry *entry = &records->entries[slot];
        /* Counts were bounded by the metadata decoder. Delete only keys owned by
         * this exact run. A missing row causes rollback rather than hiding an
         * incomplete archive; corruption can be diagnosed without losing data. */
        for (size_t i = 0; status == UMI_STATUS_OK && i < entry->run.completed; ++i)
        {
            row_key(archive, id, "result", i, key);
            status = UmiTestArchiveValueDelete(archive->server, key);
        }
        for (size_t i = 0; status == UMI_STATUS_OK && i < entry->plan.request_count; ++i)
        {
            row_key(archive, id, "request", i, key);
            status = umi_data_server_delete(archive->server, key);
        }
        if (status == UMI_STATUS_OK)
        {
            slot_key(archive, slot, key);
            status = umi_data_server_delete(archive->server, key);
        }
    }
    status = finish(archive, status);
    free(records);
    return status;
}
#endif
UmiStatus UmiTestArchiveRemoveWithCancellation(UmiTestArchive *archive, uint64_t id,
    const UmiCancellationToken *cancellation)
{
    if (archive == NULL || id == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancellation))
        return UMI_STATUS_CANCELLED;
    ArchiveRecords *records = NULL;
    UmiStatus status = start(archive, &records);
    if (status != UMI_STATUS_OK)
        return status;
    size_t slot = find(records, id);
    if (slot == UMI_TEST_ARCHIVE_CAPACITY)
        status = UMI_STATUS_NOT_FOUND;
    char key[160];
    if (status == UMI_STATUS_OK)
    {
        const UmiTestArchiveEntry *entry = &records->entries[slot];
        /* Counts were bounded by the metadata decoder. Delete only keys owned by
         * this exact run. A missing row causes rollback rather than hiding an
         * incomplete archive; corruption can be diagnosed without losing data. */
        for (size_t i = 0; status == UMI_STATUS_OK && i < entry->run.completed; ++i)
        {
            if (umi_cancellation_token_is_requested(cancellation))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            row_key(archive, id, "result", i, key);
            status = UmiTestArchiveValueDelete(archive->server, key);
        }
        for (size_t i = 0; status == UMI_STATUS_OK && i < entry->plan.request_count; ++i)
        {
            if (umi_cancellation_token_is_requested(cancellation))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            row_key(archive, id, "request", i, key);
            status = umi_data_server_delete(archive->server, key);
        }
        if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancellation))
            status = UMI_STATUS_CANCELLED;
        if (status == UMI_STATUS_OK)
        {
            slot_key(archive, slot, key);
            status = umi_data_server_delete(archive->server, key);
        }
    }
    /* Cancellation before commit rolls back every owned row. After commit, a
     * late request cannot undo the removal and must not hide that outcome. */
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancellation))
        status = UMI_STATUS_CANCELLED;
    status = finish(archive, status);
    free(records);
    return status;
}
/* Existing headless callers keep the same transactional removal contract. */
UmiStatus UmiTestArchiveRemove(UmiTestArchive *archive, uint64_t id)
{
    return UmiTestArchiveRemoveWithCancellation(archive, id, NULL);
}

/* Comparisons reuse the transaction owner above so both runs are observed together. */
#include "archive_compare.inc"
