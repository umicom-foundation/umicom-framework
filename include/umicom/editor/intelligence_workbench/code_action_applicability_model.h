/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_action_applicability_model.h
 *
 * PURPOSE:
 *   Model code action applicability model as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_APPLICABILITY_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_APPLICABILITY_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelCodeActionApplicabilityModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelCodeActionApplicabilityModel;
UmiStatus umi_editor_intel_code_action_applicability_model_init(UmiEditorIntelCodeActionApplicabilityModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_code_action_applicability_model_set_score(UmiEditorIntelCodeActionApplicabilityModel *model,uint32_t score);
UmiStatus umi_editor_intel_code_action_applicability_model_set_selected(UmiEditorIntelCodeActionApplicabilityModel *model,bool selected);
int umi_editor_intel_code_action_applicability_model_valid(const UmiEditorIntelCodeActionApplicabilityModel *model);

#ifdef __cplusplus
}
#endif
#endif
