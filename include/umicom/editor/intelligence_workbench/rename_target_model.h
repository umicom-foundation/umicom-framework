/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/rename_target_model.h
 *
 * PURPOSE:
 *   Model rename target model as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_TARGET_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_TARGET_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRenameTargetModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRenameTargetModel;
UmiStatus umi_editor_intel_rename_target_model_init(UmiEditorIntelRenameTargetModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_rename_target_model_set_score(UmiEditorIntelRenameTargetModel *model,uint32_t score);
UmiStatus umi_editor_intel_rename_target_model_set_selected(UmiEditorIntelRenameTargetModel *model,bool selected);
int umi_editor_intel_rename_target_model_valid(const UmiEditorIntelRenameTargetModel *model);

#ifdef __cplusplus
}
#endif
#endif
