/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/audit.c
 *
 * PURPOSE:
 *   Implement bounded audit retention without source/prompt persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/audit.h"
#include "umicom/base/snapshot_validation.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiCodingAuditLog {
    UmiAiCodingAuditRecord items[UMI_AI_CODING_AUDIT_CAPACITY];
    size_t count;
    uint64_t sequence;
};

/* A bounded record remains historical evidence. Text admission does not
 * reinterpret old status, state, hash or line-count values as current policy. */
UmiStatus UmiAiCodingAuditRecordValidateText(const UmiAiCodingAuditRecord *record)
{
    static const UmiSnapshotTextField fields[] = {
        {"task_id", offsetof(UmiAiCodingAuditRecord, task_id), sizeof(((UmiAiCodingAuditRecord *)0)->task_id), 0},
        {"summary", offsetof(UmiAiCodingAuditRecord, summary), sizeof(((UmiAiCodingAuditRecord *)0)->summary), 0}
    };
    return UmiSnapshotValidateTextFields(record, sizeof(*record), fields,
        sizeof(fields) / sizeof(fields[0]), NULL);
}

/* Retention always keeps a consecutive suffix. Check its boundaries before
 * pointer arithmetic so an invalid owner cannot evict or expose unrelated data. */
static UmiStatus audit_log_check(const UmiAiCodingAuditLog *log)
{
    if (log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (log->count > UMI_AI_CODING_AUDIT_CAPACITY ||
        log->sequence < log->count ||
        (log->count == 0U && log->sequence != 0U)) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < log->count; ++i)
        if (log->items[i].sequence != log->sequence - (uint64_t)(log->count - 1U - i))
            return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* Share read-only admission with composed observation publication so all owners can be checked before any retention changes. The former implementation is retained for owner review. */
#if 0
UmiStatus UmiAiCodingAuditLogAppendBatch(UmiAiCodingAuditLog *log,
    const UmiAiCodingAuditRecord *records, size_t count)
{
    UmiStatus status = audit_log_check(log);
    if (status != UMI_STATUS_OK) return status;
    if (count > UMI_AI_CODING_AUDIT_CAPACITY ||
        (uint64_t)count > UINT64_MAX - log->sequence) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (count != 0U && records == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < count; ++i) {
        status = UmiAiCodingAuditRecordValidateText(&records[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    if (count == 0U) return UMI_STATUS_OK;
    size_t room = UMI_AI_CODING_AUDIT_CAPACITY - log->count;
    size_t dropped = count > room ? count - room : 0U;
    size_t retained = log->count - dropped;
    if (dropped != 0U)
        memmove(log->items, log->items + dropped, retained * sizeof(log->items[0]));
    /* The complete group is admitted: no refusal remains after eviction. */
    for (size_t i = 0U; i < count; ++i) {
        log->items[retained + i] = records[i];
        log->items[retained + i].sequence = ++log->sequence;
    }
    log->count = retained + count;
    return UMI_STATUS_OK;
}

#endif
/* Read-only admission shares exactly the checks used by the mutator.
 * Serialize the owner until the matching operation completes; this is not a reservation. */
UmiStatus UmiAiCodingAuditLogCheckAppendBatch(const UmiAiCodingAuditLog *log,
    const UmiAiCodingAuditRecord *records, size_t count)
{
    UmiStatus status = audit_log_check(log);
    if (status != UMI_STATUS_OK) return status;
    if (count > UMI_AI_CODING_AUDIT_CAPACITY ||
        (uint64_t)count > UINT64_MAX - log->sequence) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (count != 0U && records == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < count; ++i) {
        status = UmiAiCodingAuditRecordValidateText(&records[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiCodingAuditLogAppendBatch(UmiAiCodingAuditLog *log,
    const UmiAiCodingAuditRecord *records, size_t count)
{
    UmiStatus status = UmiAiCodingAuditLogCheckAppendBatch(log, records, count);
    if (status != UMI_STATUS_OK) return status;
    if (count == 0U) return UMI_STATUS_OK;
    size_t room = UMI_AI_CODING_AUDIT_CAPACITY - log->count;
    size_t dropped = count > room ? count - room : 0U;
    size_t retained = log->count - dropped;
    if (dropped != 0U)
        memmove(log->items, log->items + dropped, retained * sizeof(log->items[0]));
    /* The complete group is admitted: no refusal remains after eviction. */
    for (size_t i = 0U; i < count; ++i) {
        log->items[retained + i] = records[i];
        log->items[retained + i].sequence = ++log->sequence;
    }
    log->count = retained + count;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiCodingAuditLogReadSince(const UmiAiCodingAuditLog *log,
    uint64_t afterSequence, UmiAiCodingAuditRecord *records, size_t capacity,
    UmiAiCodingAuditRead *outRead)
{
    UmiStatus status = audit_log_check(log);
    if (status != UMI_STATUS_OK) return status;
    if (outRead == NULL || (capacity != 0U && records == NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    if (afterSequence > log->sequence) return UMI_STATUS_INVALID_STATE;
    UmiAiCodingAuditRead result = {0};
    result.newest_sequence = log->sequence;
    result.next_sequence = afterSequence;
    if (log->count != 0U) {
        uint64_t oldest = log->items[0].sequence;
        result.oldest_sequence = oldest;
        if (afterSequence < oldest) result.missed_records = oldest - afterSequence - 1U;
        size_t start = afterSequence < oldest ? 0U : (size_t)(afterSequence - oldest + 1U);
        size_t available = log->count - start;
        size_t count = available < capacity ? available : capacity;
        for (size_t i = 0U; i < count; ++i)
            if (UmiAiCodingAuditRecordValidateText(&log->items[start + i]) != UMI_STATUS_OK)
                return UMI_STATUS_INVALID_STATE;
        /* Validate every selected record before changing either output. */
        if (count != 0U) {
            memcpy(records, log->items + start, count * sizeof(records[0]));
            result.next_sequence = log->items[start + count - 1U].sequence;
        }
        result.count = count;
        result.has_more = count < available;
    }
    *outRead = result;
    return UMI_STATUS_OK;
}

/*
 * Initialise ai coding audit log from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_audit_log_create(UmiAiCodingAuditLog **out_log)
{
    UmiAiCodingAuditLog *log;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_log = NULL;

    log = (UmiAiCodingAuditLog *)calloc(1U, sizeof(*log));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_log = log;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai coding audit log so the same storage can be reused
 * safely.
 */
void umi_ai_coding_audit_log_destroy(UmiAiCodingAuditLog *log)
{
    free(log);
}

/* Add ai coding audit log only after its inputs and available capacity have been checked. */
/* Batch admission now checks bounded text, retention state and identity headroom before eviction. The single-record interface delegates to the same service. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_audit_log_append(
    UmiAiCodingAuditLog *log,
    const UmiAiCodingAuditRecord *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL || record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (log->count == UMI_AI_CODING_AUDIT_CAPACITY) {
        (void)memmove(
            &log->items[0],
            &log->items[1],
            (log->count - 1U) * sizeof(log->items[0]));
        log->count -= 1U;
    }

    log->items[log->count] = *record;
    log->items[log->count].sequence = ++log->sequence;
    log->count += 1U;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_audit_log_append(
    UmiAiCodingAuditLog *log,
    const UmiAiCodingAuditRecord *record)
{
    return UmiAiCodingAuditLogAppendBatch(log, record, 1U);
}

/*
 * Find ai coding audit log while leaving the underlying catalogue or model owned by this
 * module.
 */
/* Bounded owner and record checks preserve positional read outputs on invalid retained evidence. Cursor reads add gap reporting without changing this compatibility interface. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_audit_log_at(
    const UmiAiCodingAuditLog *log,
    size_t index,
    UmiAiCodingAuditRecord *out_record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= log->count) return UMI_STATUS_NOT_FOUND;

    *out_record = log->items[index];
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_audit_log_at(
    const UmiAiCodingAuditLog *log,
    size_t index,
    UmiAiCodingAuditRecord *out_record)
{
    UmiStatus status = audit_log_check(log);
    if (status != UMI_STATUS_OK) return status;
    if (out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= log->count) return UMI_STATUS_NOT_FOUND;
    if (UmiAiCodingAuditRecordValidateText(&log->items[index]) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;
    *out_record = log->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ai coding audit log without changing their
 * state.
 */
size_t umi_ai_coding_audit_log_count(const UmiAiCodingAuditLog *log)
{
    return log != NULL ? log->count : 0U;
}
