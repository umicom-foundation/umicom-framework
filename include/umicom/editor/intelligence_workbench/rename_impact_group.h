/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/rename_impact_group.h
 *
 * PURPOSE:
 *   Model rename impact group as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_IMPACT_GROUP_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_IMPACT_GROUP_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel rename impact group data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRenameImpactGroup { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelRenameImpactGroup;
/**
 * Initialise editor intel rename impact group from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_rename_impact_group_init(UmiEditorIntelRenameImpactGroup *model);
/**
 * Add editor intel rename impact group only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_editor_intel_rename_impact_group_add(UmiEditorIntelRenameImpactGroup *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel rename impact group while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_rename_impact_group_find(const UmiEditorIntelRenameImpactGroup *model,const char *id);
/**
 * Release or reset state held by editor intel rename impact group so the same storage can
 * be reused safely.
 */
UmiStatus umi_editor_intel_rename_impact_group_clear(UmiEditorIntelRenameImpactGroup *model);
/**
 * Check that editor intel rename impact group satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_rename_impact_group_valid(const UmiEditorIntelRenameImpactGroup *model);

#ifdef __cplusplus
}
#endif
#endif
