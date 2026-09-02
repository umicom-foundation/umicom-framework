/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/search_filter_model.h
 *
 * PURPOSE:
 *   Model search filter model as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_FILTER_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_FILTER_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel search filter model data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelSearchFilterModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelSearchFilterModel;
/**
 * Initialise editor intel search filter model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_search_filter_model_init(UmiEditorIntelSearchFilterModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel search filter model set score operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_search_filter_model_set_score(UmiEditorIntelSearchFilterModel *model,uint32_t score);
/**
 * Find editor intel search filter model set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_intel_search_filter_model_set_selected(UmiEditorIntelSearchFilterModel *model,bool selected);
/**
 * Check that editor intel search filter model satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_search_filter_model_valid(const UmiEditorIntelSearchFilterModel *model);

#ifdef __cplusplus
}
#endif
#endif
