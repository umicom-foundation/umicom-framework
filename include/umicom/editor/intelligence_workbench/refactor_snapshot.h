/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_snapshot.h
 *
 * PURPOSE:
 *   Model refactor snapshot as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_SNAPSHOT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_SNAPSHOT_H

#include "umicom/editor/intelligence_workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel refactor snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRefactorSnapshot { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRefactorSnapshot;
/**
 * Initialise editor intel refactor snapshot from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_refactor_snapshot_init(UmiEditorIntelRefactorSnapshot *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel refactor snapshot set score operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_snapshot_set_score(UmiEditorIntelRefactorSnapshot *model,uint32_t score);
/**
 * Find editor intel refactor snapshot set while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_intel_refactor_snapshot_set_selected(UmiEditorIntelRefactorSnapshot *model,bool selected);
/**
 * Check that editor intel refactor snapshot satisfies its contract before another service
 * relies on it.
 */
int umi_editor_intel_refactor_snapshot_valid(const UmiEditorIntelRefactorSnapshot *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_intel_refactor_snapshot_archive_encode(const UmiEditorIntelRefactorSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_intel_refactor_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorIntelRefactorSnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
