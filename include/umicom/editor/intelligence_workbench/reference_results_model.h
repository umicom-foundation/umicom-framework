/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/reference_results_model.h
 *
 * PURPOSE:
 *   Model reference results model as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFERENCE_RESULTS_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFERENCE_RESULTS_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelReferenceResultsModel { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelReferenceResultsModel;
UmiStatus umi_editor_intel_reference_results_model_init(UmiEditorIntelReferenceResultsModel *model);
UmiStatus umi_editor_intel_reference_results_model_add(UmiEditorIntelReferenceResultsModel *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_reference_results_model_find(const UmiEditorIntelReferenceResultsModel *model,const char *id);
UmiStatus umi_editor_intel_reference_results_model_clear(UmiEditorIntelReferenceResultsModel *model);
int umi_editor_intel_reference_results_model_valid(const UmiEditorIntelReferenceResultsModel *model);

#ifdef __cplusplus
}
#endif
#endif
