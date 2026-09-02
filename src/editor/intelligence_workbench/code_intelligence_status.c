/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/code_intelligence_status.c
 *
 * PURPOSE:
 *   Model code intelligence status as toolkit-neutral Framework-owned editor intelligence state.
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
#include "umicom/editor/intelligence_workbench/code_intelligence_status.h"

#include <string.h>

/*
 * Provide the editor intel code intelligence status begin operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_code_intelligence_status_begin(UmiEditorIntelCodeIntelligenceStatus *session,const char *session_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(session==NULL||!umi_editor_intel_id_valid(session_id))return UMI_STATUS_INVALID_ARGUMENT;memset(session,0,sizeof *session);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_copy_text(session->session_id,sizeof session->session_id,session_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;session->phase=UMI_EDITOR_INTEL_PHASE_PREPARING;session->revision=1U;return UMI_STATUS_OK;}
/*
 * Provide the editor intel code intelligence status set ready operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_code_intelligence_status_set_ready(UmiEditorIntelCodeIntelligenceStatus *session,uint32_t item_count){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(session==NULL||session->phase!=UMI_EDITOR_INTEL_PHASE_PREPARING)return UMI_STATUS_INVALID_STATE;session->item_count=item_count;session->phase=UMI_EDITOR_INTEL_PHASE_READY;session->changed=true;session->revision++;return UMI_STATUS_OK;}
/*
 * Provide the editor intel code intelligence status cancel operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_code_intelligence_status_cancel(UmiEditorIntelCodeIntelligenceStatus *session){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(session==NULL||session->phase==UMI_EDITOR_INTEL_PHASE_COMMITTED)return UMI_STATUS_INVALID_STATE;session->phase=UMI_EDITOR_INTEL_PHASE_CANCELLED;session->revision++;return UMI_STATUS_OK;}
/*
 * Check that editor intel code intelligence status satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_code_intelligence_status_valid(const UmiEditorIntelCodeIntelligenceStatus *session){return session!=NULL&&umi_editor_intel_id_valid(session->session_id)&&session->phase>=UMI_EDITOR_INTEL_PHASE_PREPARING&&session->phase<=UMI_EDITOR_INTEL_PHASE_CANCELLED;}
