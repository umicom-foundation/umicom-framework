/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_checkpoint.h
 *
 * PURPOSE:
 *   Model refactor checkpoint as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_CHECKPOINT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_CHECKPOINT_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel refactor checkpoint data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRefactorCheckpoint { char transaction_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t total_operations; uint32_t applied_operations; uint64_t revision; } UmiEditorIntelRefactorCheckpoint;
/**
 * Provide the editor intel refactor checkpoint begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_intel_refactor_checkpoint_begin(UmiEditorIntelRefactorCheckpoint *transaction,const char *transaction_id,uint32_t total_operations);
/**
 * Perform editor intel refactor checkpoint record through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_editor_intel_refactor_checkpoint_record_apply(UmiEditorIntelRefactorCheckpoint *transaction);
/**
 * Provide the editor intel refactor checkpoint commit operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_checkpoint_commit(UmiEditorIntelRefactorCheckpoint *transaction);
/**
 * Provide the editor intel refactor checkpoint rollback operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_checkpoint_rollback(UmiEditorIntelRefactorCheckpoint *transaction);
/**
 * Check that editor intel refactor checkpoint satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_checkpoint_valid(const UmiEditorIntelRefactorCheckpoint *transaction);

#ifdef __cplusplus
}
#endif
#endif
