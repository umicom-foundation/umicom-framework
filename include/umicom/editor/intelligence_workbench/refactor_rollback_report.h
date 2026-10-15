/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_rollback_report.h
 *
 * PURPOSE:
 *   Model refactor rollback report as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_ROLLBACK_REPORT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_ROLLBACK_REPORT_H

#include "umicom/editor/intelligence_workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel refactor rollback report data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelRefactorRollbackReport { char transaction_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t total_operations; uint32_t applied_operations; uint64_t revision; } UmiEditorIntelRefactorRollbackReport;
/**
 * Provide the editor intel refactor rollback report begin operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_rollback_report_begin(UmiEditorIntelRefactorRollbackReport *transaction,const char *transaction_id,uint32_t total_operations);
/**
 * Perform editor intel refactor rollback report record through the module contract so
 * client applications do not duplicate its policy.
 */
UmiStatus umi_editor_intel_refactor_rollback_report_record_apply(UmiEditorIntelRefactorRollbackReport *transaction);
/**
 * Provide the editor intel refactor rollback report commit operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_rollback_report_commit(UmiEditorIntelRefactorRollbackReport *transaction);
/**
 * Provide the editor intel refactor rollback report rollback operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_rollback_report_rollback(UmiEditorIntelRefactorRollbackReport *transaction);
/**
 * Check that editor intel refactor rollback report satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_rollback_report_valid(const UmiEditorIntelRefactorRollbackReport *transaction);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_intel_refactor_rollback_report_archive_encode(const UmiEditorIntelRefactorRollbackReport *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_intel_refactor_rollback_report_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorIntelRefactorRollbackReport *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_editor_intel_refactor_rollback_report_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiEditorIntelRefactorRollbackReport *expected, UmiEditorIntelRefactorRollbackReport *value);


#ifdef __cplusplus
}
#endif
#endif
