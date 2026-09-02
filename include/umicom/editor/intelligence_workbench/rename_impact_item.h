/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/rename_impact_item.h
 *
 * PURPOSE:
 *   Model rename impact item as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_IMPACT_ITEM_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_IMPACT_ITEM_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel rename impact item data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRenameImpactItem { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRenameImpactItem;
/**
 * Initialise editor intel rename impact item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_rename_impact_item_init(UmiEditorIntelRenameImpactItem *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel rename impact item set score operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_rename_impact_item_set_score(UmiEditorIntelRenameImpactItem *model,uint32_t score);
/**
 * Find editor intel rename impact item set while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_intel_rename_impact_item_set_selected(UmiEditorIntelRenameImpactItem *model,bool selected);
/**
 * Check that editor intel rename impact item satisfies its contract before another service
 * relies on it.
 */
int umi_editor_intel_rename_impact_item_valid(const UmiEditorIntelRenameImpactItem *model);

#ifdef __cplusplus
}
#endif
#endif
