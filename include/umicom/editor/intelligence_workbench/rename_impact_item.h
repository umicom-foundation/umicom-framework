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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_IMPACT_ITEM_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_IMPACT_ITEM_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRenameImpactItem { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRenameImpactItem;
UmiStatus umi_editor_intel_rename_impact_item_init(UmiEditorIntelRenameImpactItem *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_rename_impact_item_set_score(UmiEditorIntelRenameImpactItem *model,uint32_t score);
UmiStatus umi_editor_intel_rename_impact_item_set_selected(UmiEditorIntelRenameImpactItem *model,bool selected);
int umi_editor_intel_rename_impact_item_valid(const UmiEditorIntelRenameImpactItem *model);

#ifdef __cplusplus
}
#endif
#endif
