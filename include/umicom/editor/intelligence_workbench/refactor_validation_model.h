/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_validation_model.h
 *
 * PURPOSE:
 *   Represent validation evidence produced before a refactor may commit.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_VALIDATION_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_VALIDATION_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRefactorValidationModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRefactorValidationModel;
UmiStatus umi_editor_intel_refactor_validation_model_init(UmiEditorIntelRefactorValidationModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_refactor_validation_model_set_score(UmiEditorIntelRefactorValidationModel *model,uint32_t score);
UmiStatus umi_editor_intel_refactor_validation_model_set_selected(UmiEditorIntelRefactorValidationModel *model,bool selected);
int umi_editor_intel_refactor_validation_model_valid(const UmiEditorIntelRefactorValidationModel *model);

#ifdef __cplusplus
}
#endif
#endif
