/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/audit.h
 *
 * PURPOSE:
 *   Define immutable audit evidence for consequential commands, policy decisions
 *   and data mutations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * List the named audit outcome values accepted by this public contract.
 */
typedef enum UmiAuditOutcome { UMI_AUDIT_SUCCEEDED = 1, UMI_AUDIT_DENIED = 2, UMI_AUDIT_FAILED = 3 } UmiAuditOutcome;
/**
 * Represent the audit record data shared with callers of this public contract.
 */
typedef struct UmiAuditRecord {
    uint64_t sequence;
    uint64_t timestamp_ns;
    uint64_t correlation_id;
    char principal[UMI_AUDIT_TEXT_CAPACITY];
    char action[UMI_AUDIT_TEXT_CAPACITY];
    char resource[UMI_AUDIT_TEXT_CAPACITY];
    UmiAuditOutcome outcome;
} UmiAuditRecord;
/**
 * Represent the audit log data shared with callers of this public contract.
 */
typedef struct UmiAuditLog UmiAuditLog;
/**
 * Initialise audit log from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_audit_log_create(UmiAuditLog **out_log);
/**
 * Release or reset state held by audit log so the same storage can be reused safely.
 */
void umi_audit_log_destroy(UmiAuditLog *log);
/**
 * Add audit log only after its inputs and available capacity have been checked.
 */
UmiStatus umi_audit_log_append(UmiAuditLog *log, const UmiAuditRecord *record);
/**
 * Return the number of records represented by audit log without changing their state.
 */
size_t umi_audit_log_count(const UmiAuditLog *log);
/**
 * Find audit log while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_audit_log_at(const UmiAuditLog *log, size_t index,
                           UmiAuditRecord *out_record);
/**
 * Return the number of records represented by audit log outcome without changing their
 * state.
 */
size_t umi_audit_log_outcome_count(const UmiAuditLog *log, UmiAuditOutcome outcome);
#ifdef __cplusplus
}
#endif

#endif
