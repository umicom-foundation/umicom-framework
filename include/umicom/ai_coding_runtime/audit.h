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

typedef struct UmiAiCodingAuditLog UmiAiCodingAuditLog;

UmiStatus umi_ai_coding_audit_log_create(UmiAiCodingAuditLog **out_log);
void umi_ai_coding_audit_log_destroy(UmiAiCodingAuditLog *log);
UmiStatus umi_ai_coding_audit_log_append(
    UmiAiCodingAuditLog *log,
    const UmiAiCodingAuditRecord *record);
UmiStatus umi_ai_coding_audit_log_at(
    const UmiAiCodingAuditLog *log,
    size_t index,
    UmiAiCodingAuditRecord *out_record);
size_t umi_ai_coding_audit_log_count(const UmiAiCodingAuditLog *log);

#ifdef __cplusplus
}
#endif
#endif
