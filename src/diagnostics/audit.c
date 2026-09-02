/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/audit.c
 *
 * PURPOSE:
 *   Implement a bounded chronological audit log and outcome counters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/audit.h"

#include <stddef.h>
#include <stdlib.h>
#include "diagnostic_lock.h"
struct UmiAuditLog { UmiAuditRecord items[UMI_AUDIT_LOG_MAX]; size_t count; size_t head; uint64_t sequence; size_t outcomes[4]; UmiDiagnosticLock lock; };
/*
 * Initialise audit log from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_audit_log_create(UmiAuditLog **out_log)
{
    UmiAuditLog *log;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_log = NULL; log = (UmiAuditLog *)calloc(1U, sizeof(*log)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_diagnostic_lock_init(&log->lock);
    log->sequence = 1U; *out_log = log; return UMI_STATUS_OK;
}
/* Release or reset state held by audit log so the same storage can be reused safely. */
void umi_audit_log_destroy(UmiAuditLog *log) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (log != NULL) { free(log); } }
/* Add audit log only after its inputs and available capacity have been checked. */
UmiStatus umi_audit_log_append(UmiAuditLog *log, const UmiAuditRecord *record)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL || record == NULL || record->action[0] == '\0' || record->outcome < UMI_AUDIT_SUCCEEDED || record->outcome > UMI_AUDIT_FAILED) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&log->lock);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (log->count < UMI_AUDIT_LOG_MAX) { index = (log->head + log->count) % UMI_AUDIT_LOG_MAX; ++log->count; }
    /* Use this fallback path when the earlier condition does not apply. */
    else { index = log->head; log->head = (log->head + 1U) % UMI_AUDIT_LOG_MAX; }
    log->items[index] = *record; log->items[index].sequence = log->sequence++; ++log->outcomes[record->outcome];
    umi_diagnostic_lock_release(&log->lock); return UMI_STATUS_OK;
}
/* Return the number of records represented by audit log without changing their state. */
size_t umi_audit_log_count(const UmiAuditLog *log)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (log != NULL) { umi_diagnostic_lock_acquire(&log->lock); n = log->count; umi_diagnostic_lock_release(&log->lock); } return n; }
/* Find audit log while leaving the underlying catalogue or model owned by this module. */
UmiStatus umi_audit_log_at(const UmiAuditLog *log, size_t index, UmiAuditRecord *out_record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&log->lock);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= log->count) { umi_diagnostic_lock_release(&log->lock); return UMI_STATUS_NOT_FOUND; }
    *out_record = log->items[(log->head + index) % UMI_AUDIT_LOG_MAX]; umi_diagnostic_lock_release(&log->lock); return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by audit log outcome without changing their
 * state.
 */
size_t umi_audit_log_outcome_count(const UmiAuditLog *log, UmiAuditOutcome outcome)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (log != NULL && outcome >= UMI_AUDIT_SUCCEEDED && outcome <= UMI_AUDIT_FAILED) { umi_diagnostic_lock_acquire(&log->lock); n = log->outcomes[outcome]; umi_diagnostic_lock_release(&log->lock); } return n; }
