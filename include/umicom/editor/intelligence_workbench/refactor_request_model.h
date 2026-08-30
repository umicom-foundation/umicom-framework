/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_request_model.h
 *
 * PURPOSE:
 *   Model refactor request model as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_REQUEST_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_REQUEST_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRefactorRequestModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRefactorRequestModel;
UmiStatus umi_editor_intel_refactor_request_model_init(UmiEditorIntelRefactorRequestModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_refactor_request_model_set_score(UmiEditorIntelRefactorRequestModel *model,uint32_t score);
UmiStatus umi_editor_intel_refactor_request_model_set_selected(UmiEditorIntelRefactorRequestModel *model,bool selected);
int umi_editor_intel_refactor_request_model_valid(const UmiEditorIntelRefactorRequestModel *model);

#ifdef __cplusplus
}
#endif
#endif
