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

/**
 * Represent the editor intel code action applicability model data shared with callers of
 * this public contract.
 */
typedef struct UmiEditorIntelCodeActionApplicabilityModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelCodeActionApplicabilityModel;
/**
 * Initialise editor intel code action applicability model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_editor_intel_code_action_applicability_model_init(UmiEditorIntelCodeActionApplicabilityModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel code action applicability model set score operation used by
 * this module and its client applications.
 */
UmiStatus umi_editor_intel_code_action_applicability_model_set_score(UmiEditorIntelCodeActionApplicabilityModel *model,uint32_t score);
/**
 * Find editor intel code action applicability model set while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_editor_intel_code_action_applicability_model_set_selected(UmiEditorIntelCodeActionApplicabilityModel *model,bool selected);
/**
 * Check that editor intel code action applicability model satisfies its contract before
 * another service relies on it.
 */
int umi_editor_intel_code_action_applicability_model_valid(const UmiEditorIntelCodeActionApplicabilityModel *model);

#ifdef __cplusplus
}
#endif
#endif
