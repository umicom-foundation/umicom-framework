/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_progress_model.h
 *
 * PURPOSE:
 *   Model refactor progress model as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_PROGRESS_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_PROGRESS_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRefactorProgressModel { char session_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t item_count; bool changed; uint64_t revision; } UmiEditorIntelRefactorProgressModel;
UmiStatus umi_editor_intel_refactor_progress_model_begin(UmiEditorIntelRefactorProgressModel *session,const char *session_id);
UmiStatus umi_editor_intel_refactor_progress_model_set_ready(UmiEditorIntelRefactorProgressModel *session,uint32_t item_count);
UmiStatus umi_editor_intel_refactor_progress_model_cancel(UmiEditorIntelRefactorProgressModel *session);
int umi_editor_intel_refactor_progress_model_valid(const UmiEditorIntelRefactorProgressModel *session);

#ifdef __cplusplus
}
#endif
#endif
