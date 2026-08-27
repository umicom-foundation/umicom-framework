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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_IMPACT_GROUP_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_IMPACT_GROUP_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRenameImpactGroup { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelRenameImpactGroup;
UmiStatus umi_editor_intel_rename_impact_group_init(UmiEditorIntelRenameImpactGroup *model);
UmiStatus umi_editor_intel_rename_impact_group_add(UmiEditorIntelRenameImpactGroup *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_rename_impact_group_find(const UmiEditorIntelRenameImpactGroup *model,const char *id);
UmiStatus umi_editor_intel_rename_impact_group_clear(UmiEditorIntelRenameImpactGroup *model);
int umi_editor_intel_rename_impact_group_valid(const UmiEditorIntelRenameImpactGroup *model);

#ifdef __cplusplus
}
#endif
#endif
