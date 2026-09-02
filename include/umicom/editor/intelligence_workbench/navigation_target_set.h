/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/navigation_target_set.h
 *
 * PURPOSE:
 *   Deduplicate and order navigation targets before presentation.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_NAVIGATION_TARGET_SET_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_NAVIGATION_TARGET_SET_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel navigation target set data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelNavigationTargetSet { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelNavigationTargetSet;
/**
 * Initialise editor intel navigation target set from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_navigation_target_set_init(UmiEditorIntelNavigationTargetSet *model);
/**
 * Add editor intel navigation target set only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_editor_intel_navigation_target_set_add(UmiEditorIntelNavigationTargetSet *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel navigation target set while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_navigation_target_set_find(const UmiEditorIntelNavigationTargetSet *model,const char *id);
/**
 * Release or reset state held by editor intel navigation target set so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_intel_navigation_target_set_clear(UmiEditorIntelNavigationTargetSet *model);
/**
 * Check that editor intel navigation target set satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_navigation_target_set_valid(const UmiEditorIntelNavigationTargetSet *model);

#ifdef __cplusplus
}
#endif
#endif
