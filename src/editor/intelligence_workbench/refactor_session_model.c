/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/refactor_session_model.c
 *
 * PURPOSE:
 *   Model refactor session model as toolkit-neutral Framework-owned editor intelligence state.
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
#include "umicom/editor/intelligence_workbench/refactor_session_model.h"

#include <string.h>

UmiStatus umi_editor_intel_refactor_session_model_begin(UmiEditorIntelRefactorSessionModel *session,const char *session_id){if(session==NULL||!umi_editor_intel_id_valid(session_id))return UMI_STATUS_INVALID_ARGUMENT;memset(session,0,sizeof *session);if(umi_editor_intel_copy_text(session->session_id,sizeof session->session_id,session_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;session->phase=UMI_EDITOR_INTEL_PHASE_PREPARING;session->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_editor_intel_refactor_session_model_set_ready(UmiEditorIntelRefactorSessionModel *session,uint32_t item_count){if(session==NULL||session->phase!=UMI_EDITOR_INTEL_PHASE_PREPARING)return UMI_STATUS_INVALID_STATE;session->item_count=item_count;session->phase=UMI_EDITOR_INTEL_PHASE_READY;session->changed=true;session->revision++;return UMI_STATUS_OK;}
UmiStatus umi_editor_intel_refactor_session_model_cancel(UmiEditorIntelRefactorSessionModel *session){if(session==NULL||session->phase==UMI_EDITOR_INTEL_PHASE_COMMITTED)return UMI_STATUS_INVALID_STATE;session->phase=UMI_EDITOR_INTEL_PHASE_CANCELLED;session->revision++;return UMI_STATUS_OK;}
int umi_editor_intel_refactor_session_model_valid(const UmiEditorIntelRefactorSessionModel *session){return session!=NULL&&umi_editor_intel_id_valid(session->session_id)&&session->phase>=UMI_EDITOR_INTEL_PHASE_PREPARING&&session->phase<=UMI_EDITOR_INTEL_PHASE_CANCELLED;}
