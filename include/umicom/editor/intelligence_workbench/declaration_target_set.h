/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/declaration_target_set.h
 *
 * PURPOSE:
 *   Model declaration target set as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_DECLARATION_TARGET_SET_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_DECLARATION_TARGET_SET_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel declaration target set data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelDeclarationTargetSet { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelDeclarationTargetSet;
/**
 * Initialise editor intel declaration target set from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_declaration_target_set_init(UmiEditorIntelDeclarationTargetSet *model);
/**
 * Add editor intel declaration target set only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_editor_intel_declaration_target_set_add(UmiEditorIntelDeclarationTargetSet *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel declaration target set while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_declaration_target_set_find(const UmiEditorIntelDeclarationTargetSet *model,const char *id);
/**
 * Release or reset state held by editor intel declaration target set so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_intel_declaration_target_set_clear(UmiEditorIntelDeclarationTargetSet *model);
/**
 * Check that editor intel declaration target set satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_declaration_target_set_valid(const UmiEditorIntelDeclarationTargetSet *model);

#ifdef __cplusplus
}
#endif
#endif
