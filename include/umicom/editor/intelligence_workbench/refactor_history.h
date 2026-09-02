/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_history.h
 *
 * PURPOSE:
 *   Model refactor history as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_HISTORY_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_HISTORY_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel refactor history data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRefactorHistory { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelRefactorHistory;
/**
 * Initialise editor intel refactor history from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_intel_refactor_history_init(UmiEditorIntelRefactorHistory *model);
/**
 * Add editor intel refactor history only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_intel_refactor_history_add(UmiEditorIntelRefactorHistory *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel refactor history while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_refactor_history_find(const UmiEditorIntelRefactorHistory *model,const char *id);
/**
 * Release or reset state held by editor intel refactor history so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_intel_refactor_history_clear(UmiEditorIntelRefactorHistory *model);
/**
 * Check that editor intel refactor history satisfies its contract before another service
 * relies on it.
 */
int umi_editor_intel_refactor_history_valid(const UmiEditorIntelRefactorHistory *model);

#ifdef __cplusplus
}
#endif
#endif
