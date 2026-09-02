/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/audit.h
 *
 * PURPOSE:
 *   Retain compact audit records for coding-agent provider, approval, mutation,
 *   validation and rollback operations without persisting source text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_AUDIT_H
#define UMICOM_AI_CODING_RUNTIME_AUDIT_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CODING_AUDIT_CAPACITY 128U

/**
 * Represent the ai coding audit record data shared with callers of this public contract.
 */
typedef struct UmiAiCodingAuditRecord {
    uint64_t sequence;
    char task_id[UMI_AI_CODING_RUNTIME_ID_CAPACITY];
    UmiAiCodingEventKind kind;
    UmiAiCodingRuntimeState state;
    UmiStatus status;
    uint64_t plan_hash;
    size_t file_count;
    uint32_t changed_lines;
    char summary[512];
} UmiAiCodingAuditRecord;

/**
 * Represent the ai coding audit log data shared with callers of this public contract.
 */
typedef struct UmiAiCodingAuditLog UmiAiCodingAuditLog;

/**
 * Initialise ai coding audit log from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_audit_log_create(UmiAiCodingAuditLog **out_log);
/**
 * Release or reset state held by ai coding audit log so the same storage can be reused
 * safely.
 */
void umi_ai_coding_audit_log_destroy(UmiAiCodingAuditLog *log);
/**
 * Add ai coding audit log only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ai_coding_audit_log_append(
    UmiAiCodingAuditLog *log,
    const UmiAiCodingAuditRecord *record);
/**
 * Find ai coding audit log while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_coding_audit_log_at(
    const UmiAiCodingAuditLog *log,
    size_t index,
    UmiAiCodingAuditRecord *out_record);
/**
 * Return the number of records represented by ai coding audit log without changing their
 * state.
 */
size_t umi_ai_coding_audit_log_count(const UmiAiCodingAuditLog *log);

#ifdef __cplusplus
}
#endif
#endif
