/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_preview_model.h
 *
 * PURPOSE:
 *   Collect previewable refactoring changes while preserving source locations.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_PREVIEW_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_PREVIEW_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRefactorPreviewModel { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelRefactorPreviewModel;
UmiStatus umi_editor_intel_refactor_preview_model_init(UmiEditorIntelRefactorPreviewModel *model);
UmiStatus umi_editor_intel_refactor_preview_model_add(UmiEditorIntelRefactorPreviewModel *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_refactor_preview_model_find(const UmiEditorIntelRefactorPreviewModel *model,const char *id);
UmiStatus umi_editor_intel_refactor_preview_model_clear(UmiEditorIntelRefactorPreviewModel *model);
int umi_editor_intel_refactor_preview_model_valid(const UmiEditorIntelRefactorPreviewModel *model);

#ifdef __cplusplus
}
#endif
#endif
