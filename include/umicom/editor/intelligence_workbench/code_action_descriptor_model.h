/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_action_descriptor_model.h
 *
 * PURPOSE:
 *   Model code action descriptor model as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_DESCRIPTOR_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_DESCRIPTOR_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelCodeActionDescriptorModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelCodeActionDescriptorModel;
UmiStatus umi_editor_intel_code_action_descriptor_model_init(UmiEditorIntelCodeActionDescriptorModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_code_action_descriptor_model_set_score(UmiEditorIntelCodeActionDescriptorModel *model,uint32_t score);
UmiStatus umi_editor_intel_code_action_descriptor_model_set_selected(UmiEditorIntelCodeActionDescriptorModel *model,bool selected);
int umi_editor_intel_code_action_descriptor_model_valid(const UmiEditorIntelCodeActionDescriptorModel *model);

#ifdef __cplusplus
}
#endif
#endif
