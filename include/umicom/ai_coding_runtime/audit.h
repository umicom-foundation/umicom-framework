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

/* Validate only fixed text termination. Empty task/summary and historical
 * scalar values remain representable; this does not authorize an operation. */
UmiStatus UmiAiCodingAuditRecordValidateText(const UmiAiCodingAuditRecord *record);

/* Admit up to UMI_AI_CODING_AUDIT_CAPACITY records as one group. Refusal
 * preserves all retained records and sequence state. New identities replace
 * caller-supplied sequence values and never wrap. A zero count is a no-op.
 * Input records must be separate from owner storage; serialize all access. */
UmiStatus UmiAiCodingAuditLogAppendBatch(UmiAiCodingAuditLog *log,
    const UmiAiCodingAuditRecord *records, size_t count);

typedef struct UmiAiCodingAuditRead {
    size_t count;
    uint64_t oldest_sequence;
    uint64_t newest_sequence;
    uint64_t next_sequence;
    uint64_t missed_records;
    int has_more;
} UmiAiCodingAuditRead;

/* Copy records strictly after afterSequence, oldest first, without consuming
 * them. Start at zero. Advance a consumer cursor to next_sequence only after
 * it has accepted the returned records. missed_records counts evicted records
 * between the supplied cursor and the oldest retained record; do not hide it.
 * A cursor newer than this log returns INVALID_STATE. Cursors belong to one
 * live log instance, not another log or a later process. No reset is provided.
 * capacity zero permits records NULL and returns metadata without advancing.
 * Output arrays, report and owner storage must be separate. Every refusal
 * preserves both outputs. Callers serialize reads, appends and destruction. */
UmiStatus UmiAiCodingAuditLogReadSince(const UmiAiCodingAuditLog *log,
    uint64_t afterSequence, UmiAiCodingAuditRecord *records, size_t capacity,
    UmiAiCodingAuditRead *outRead);

/* Predict the matching batch mutation without changing this owner.
 * A successful check is valid only while the owner and inputs remain unchanged
 * on the serialized owner thread. It reserves no capacity and acquires no lock. */
UmiStatus UmiAiCodingAuditLogCheckAppendBatch(const UmiAiCodingAuditLog *log,
    const UmiAiCodingAuditRecord *records, size_t count);

#ifdef __cplusplus
}
#endif
#endif
