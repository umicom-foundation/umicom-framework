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
UmiStatus umi_audit_log_create(UmiAuditLog **out_log)
{
    UmiAuditLog *log;
    if (out_log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_log = NULL; log = (UmiAuditLog *)calloc(1U, sizeof(*log)); if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_diagnostic_lock_init(&log->lock);
    log->sequence = 1U; *out_log = log; return UMI_STATUS_OK;
}
void umi_audit_log_destroy(UmiAuditLog *log) { if (log != NULL) { free(log); } }
UmiStatus umi_audit_log_append(UmiAuditLog *log, const UmiAuditRecord *record)
{
    size_t index;
    if (log == NULL || record == NULL || record->action[0] == '\0' || record->outcome < UMI_AUDIT_SUCCEEDED || record->outcome > UMI_AUDIT_FAILED) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&log->lock);
    if (log->count < UMI_AUDIT_LOG_MAX) { index = (log->head + log->count) % UMI_AUDIT_LOG_MAX; ++log->count; }
    else { index = log->head; log->head = (log->head + 1U) % UMI_AUDIT_LOG_MAX; }
    log->items[index] = *record; log->items[index].sequence = log->sequence++; ++log->outcomes[record->outcome];
    umi_diagnostic_lock_release(&log->lock); return UMI_STATUS_OK;
}
size_t umi_audit_log_count(const UmiAuditLog *log)
{ size_t n = 0U; if (log != NULL) { umi_diagnostic_lock_acquire(&log->lock); n = log->count; umi_diagnostic_lock_release(&log->lock); } return n; }
UmiStatus umi_audit_log_at(const UmiAuditLog *log, size_t index, UmiAuditRecord *out_record)
{
    if (log == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&log->lock);
    if (index >= log->count) { umi_diagnostic_lock_release(&log->lock); return UMI_STATUS_NOT_FOUND; }
    *out_record = log->items[(log->head + index) % UMI_AUDIT_LOG_MAX]; umi_diagnostic_lock_release(&log->lock); return UMI_STATUS_OK;
}
size_t umi_audit_log_outcome_count(const UmiAuditLog *log, UmiAuditOutcome outcome)
{ size_t n = 0U; if (log != NULL && outcome >= UMI_AUDIT_SUCCEEDED && outcome <= UMI_AUDIT_FAILED) { umi_diagnostic_lock_acquire(&log->lock); n = log->outcomes[outcome]; umi_diagnostic_lock_release(&log->lock); } return n; }
