/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/audit.h
 *
 * PURPOSE:
 *   Define immutable audit evidence for consequential commands, policy decisions
 *   and data mutations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_AUDIT_H
#define UMICOM_DIAGNOSTICS_AUDIT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AUDIT_TEXT_CAPACITY 192U
#define UMI_AUDIT_LOG_MAX 4096U
typedef enum UmiAuditOutcome { UMI_AUDIT_SUCCEEDED = 1, UMI_AUDIT_DENIED = 2, UMI_AUDIT_FAILED = 3 } UmiAuditOutcome;
typedef struct UmiAuditRecord {
    uint64_t sequence;
    uint64_t timestamp_ns;
    uint64_t correlation_id;
    char principal[UMI_AUDIT_TEXT_CAPACITY];
    char action[UMI_AUDIT_TEXT_CAPACITY];
    char resource[UMI_AUDIT_TEXT_CAPACITY];
    UmiAuditOutcome outcome;
} UmiAuditRecord;
typedef struct UmiAuditLog UmiAuditLog;
UmiStatus umi_audit_log_create(UmiAuditLog **out_log);
void umi_audit_log_destroy(UmiAuditLog *log);
UmiStatus umi_audit_log_append(UmiAuditLog *log, const UmiAuditRecord *record);
size_t umi_audit_log_count(const UmiAuditLog *log);
UmiStatus umi_audit_log_at(const UmiAuditLog *log, size_t index,
                           UmiAuditRecord *out_record);
size_t umi_audit_log_outcome_count(const UmiAuditLog *log, UmiAuditOutcome outcome);
#ifdef __cplusplus
}
#endif

#endif
