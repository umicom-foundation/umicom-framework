/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/search_session_model.h
 *
 * PURPOSE:
 *   Model search session model as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_SESSION_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_SESSION_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelSearchSessionModel { char session_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t item_count; bool changed; uint64_t revision; } UmiEditorIntelSearchSessionModel;
UmiStatus umi_editor_intel_search_session_model_begin(UmiEditorIntelSearchSessionModel *session,const char *session_id);
UmiStatus umi_editor_intel_search_session_model_set_ready(UmiEditorIntelSearchSessionModel *session,uint32_t item_count);
UmiStatus umi_editor_intel_search_session_model_cancel(UmiEditorIntelSearchSessionModel *session);
int umi_editor_intel_search_session_model_valid(const UmiEditorIntelSearchSessionModel *session);

#ifdef __cplusplus
}
#endif
#endif
