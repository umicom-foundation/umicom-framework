/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_restore.h
 *
 * PURPOSE:
 *   Model refactor restore as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_RESTORE_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_RESTORE_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel refactor restore data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRefactorRestore { char session_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t item_count; bool changed; uint64_t revision; } UmiEditorIntelRefactorRestore;
/**
 * Provide the editor intel refactor restore begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_intel_refactor_restore_begin(UmiEditorIntelRefactorRestore *session,const char *session_id);
/**
 * Provide the editor intel refactor restore set ready operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_restore_set_ready(UmiEditorIntelRefactorRestore *session,uint32_t item_count);
/**
 * Provide the editor intel refactor restore cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_intel_refactor_restore_cancel(UmiEditorIntelRefactorRestore *session);
/**
 * Check that editor intel refactor restore satisfies its contract before another service
 * relies on it.
 */
int umi_editor_intel_refactor_restore_valid(const UmiEditorIntelRefactorRestore *session);

#ifdef __cplusplus
}
#endif
#endif
