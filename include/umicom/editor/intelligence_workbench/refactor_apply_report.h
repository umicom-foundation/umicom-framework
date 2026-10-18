/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_apply_report.h
 *
 * PURPOSE:
 *   Model refactor apply report as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_APPLY_REPORT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_APPLY_REPORT_H

#include "umicom/editor/intelligence_workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel refactor apply report data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRefactorApplyReport { char transaction_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t total_operations; uint32_t applied_operations; uint64_t revision; } UmiEditorIntelRefactorApplyReport;
/**
 * Provide the editor intel refactor apply report begin operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_apply_report_begin(UmiEditorIntelRefactorApplyReport *transaction,const char *transaction_id,uint32_t total_operations);
/**
 * Perform editor intel refactor apply report record through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_editor_intel_refactor_apply_report_record_apply(UmiEditorIntelRefactorApplyReport *transaction);
/**
 * Provide the editor intel refactor apply report commit operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_apply_report_commit(UmiEditorIntelRefactorApplyReport *transaction);
/**
 * Provide the editor intel refactor apply report rollback operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_apply_report_rollback(UmiEditorIntelRefactorApplyReport *transaction);
/**
 * Check that editor intel refactor apply report satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_apply_report_valid(const UmiEditorIntelRefactorApplyReport *transaction);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_intel_refactor_apply_report_archive_encode(const UmiEditorIntelRefactorApplyReport *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_intel_refactor_apply_report_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorIntelRefactorApplyReport *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiEditorIntelRefactorApplyReport object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_editor_intel_refactor_apply_report_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiEditorIntelRefactorApplyReport *workspace, UmiEditorIntelRefactorApplyReport *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_editor_intel_refactor_apply_report_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiEditorIntelRefactorApplyReport *expected, UmiEditorIntelRefactorApplyReport *value);


#ifdef __cplusplus
}
#endif
#endif
