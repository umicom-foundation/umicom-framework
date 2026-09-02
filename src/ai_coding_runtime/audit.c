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

#include <stdlib.h>
#include <string.h>

struct UmiAiCodingAuditLog {
    UmiAiCodingAuditRecord items[UMI_AI_CODING_AUDIT_CAPACITY];
    size_t count;
    uint64_t sequence;
};

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

/*
 * Find ai coding audit log while leaving the underlying catalogue or model owned by this
 * module.
 */
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

/*
 * Return the number of records represented by ai coding audit log without changing their
 * state.
 */
size_t umi_ai_coding_audit_log_count(const UmiAiCodingAuditLog *log)
{
    return log != NULL ? log->count : 0U;
}
