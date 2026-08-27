/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_action_preview_model.h
 *
 * PURPOSE:
 *   Represent the preview state of a resolved code action.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_PREVIEW_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_PREVIEW_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelCodeActionPreviewModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelCodeActionPreviewModel;
UmiStatus umi_editor_intel_code_action_preview_model_init(UmiEditorIntelCodeActionPreviewModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_code_action_preview_model_set_score(UmiEditorIntelCodeActionPreviewModel *model,uint32_t score);
UmiStatus umi_editor_intel_code_action_preview_model_set_selected(UmiEditorIntelCodeActionPreviewModel *model,bool selected);
int umi_editor_intel_code_action_preview_model_valid(const UmiEditorIntelCodeActionPreviewModel *model);

#ifdef __cplusplus
}
#endif
#endif
