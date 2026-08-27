/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_result_model.h
 *
 * PURPOSE:
 *   Model refactor result model as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_RESULT_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_RESULT_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRefactorResultModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRefactorResultModel;
UmiStatus umi_editor_intel_refactor_result_model_init(UmiEditorIntelRefactorResultModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_refactor_result_model_set_score(UmiEditorIntelRefactorResultModel *model,uint32_t score);
UmiStatus umi_editor_intel_refactor_result_model_set_selected(UmiEditorIntelRefactorResultModel *model,bool selected);
int umi_editor_intel_refactor_result_model_valid(const UmiEditorIntelRefactorResultModel *model);

#ifdef __cplusplus
}
#endif
#endif
