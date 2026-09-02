/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/implementation_target_set.h
 *
 * PURPOSE:
 *   Model implementation target set as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_IMPLEMENTATION_TARGET_SET_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_IMPLEMENTATION_TARGET_SET_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel implementation target set data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelImplementationTargetSet { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelImplementationTargetSet;
/**
 * Initialise editor intel implementation target set from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_implementation_target_set_init(UmiEditorIntelImplementationTargetSet *model);
/**
 * Add editor intel implementation target set only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_editor_intel_implementation_target_set_add(UmiEditorIntelImplementationTargetSet *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel implementation target set while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_implementation_target_set_find(const UmiEditorIntelImplementationTargetSet *model,const char *id);
/**
 * Release or reset state held by editor intel implementation target set so the same
 * storage can be reused safely.
 */
UmiStatus umi_editor_intel_implementation_target_set_clear(UmiEditorIntelImplementationTargetSet *model);
/**
 * Check that editor intel implementation target set satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_implementation_target_set_valid(const UmiEditorIntelImplementationTargetSet *model);

#ifdef __cplusplus
}
#endif
#endif
