/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_file_change.h
 *
 * PURPOSE:
 *   Model refactor file change as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_FILE_CHANGE_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_FILE_CHANGE_H

#include "umicom/editor/intelligence_workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel refactor file change data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRefactorFileChange { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRefactorFileChange;
/**
 * Initialise editor intel refactor file change from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_refactor_file_change_init(UmiEditorIntelRefactorFileChange *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel refactor file change set score operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_file_change_set_score(UmiEditorIntelRefactorFileChange *model,uint32_t score);
/**
 * Find editor intel refactor file change set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_intel_refactor_file_change_set_selected(UmiEditorIntelRefactorFileChange *model,bool selected);
/**
 * Check that editor intel refactor file change satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_file_change_valid(const UmiEditorIntelRefactorFileChange *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_intel_refactor_file_change_archive_encode(const UmiEditorIntelRefactorFileChange *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_intel_refactor_file_change_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorIntelRefactorFileChange *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_editor_intel_refactor_file_change_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiEditorIntelRefactorFileChange *expected, UmiEditorIntelRefactorFileChange *value);


#ifdef __cplusplus
}
#endif
#endif
