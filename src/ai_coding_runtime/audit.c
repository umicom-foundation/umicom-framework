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

UmiStatus umi_ai_coding_audit_log_create(UmiAiCodingAuditLog **out_log)
{
    UmiAiCodingAuditLog *log;

    if (out_log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_log = NULL;

    log = (UmiAiCodingAuditLog *)calloc(1U, sizeof(*log));
    if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_log = log;
    return UMI_STATUS_OK;
}

void umi_ai_coding_audit_log_destroy(UmiAiCodingAuditLog *log)
{
    free(log);
}

UmiStatus umi_ai_coding_audit_log_append(
    UmiAiCodingAuditLog *log,
    const UmiAiCodingAuditRecord *record)
{
    if (log == NULL || record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

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

UmiStatus umi_ai_coding_audit_log_at(
    const UmiAiCodingAuditLog *log,
    size_t index,
    UmiAiCodingAuditRecord *out_record)
{
    if (log == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (index >= log->count) return UMI_STATUS_NOT_FOUND;

    *out_record = log->items[index];
    return UMI_STATUS_OK;
}

size_t umi_ai_coding_audit_log_count(const UmiAiCodingAuditLog *log)
{
    return log != NULL ? log->count : 0U;
}
