/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_action_session_model.h
 *
 * PURPOSE:
 *   Model code action session model as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_SESSION_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_SESSION_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelCodeActionSessionModel { char session_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t item_count; bool changed; uint64_t revision; } UmiEditorIntelCodeActionSessionModel;
UmiStatus umi_editor_intel_code_action_session_model_begin(UmiEditorIntelCodeActionSessionModel *session,const char *session_id);
UmiStatus umi_editor_intel_code_action_session_model_set_ready(UmiEditorIntelCodeActionSessionModel *session,uint32_t item_count);
UmiStatus umi_editor_intel_code_action_session_model_cancel(UmiEditorIntelCodeActionSessionModel *session);
int umi_editor_intel_code_action_session_model_valid(const UmiEditorIntelCodeActionSessionModel *session);

#ifdef __cplusplus
}
#endif
#endif
