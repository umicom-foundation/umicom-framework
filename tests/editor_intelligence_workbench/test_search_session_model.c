/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_search_session_model.c
 *
 * PURPOSE:
 *   Implement the test search session model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/search_session_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEditorIntelSearchSessionModel session;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_search_session_model_begin(&session,"session-1")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_search_session_model_set_ready(&session,3U)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_intel_search_session_model_valid(&session)||session.item_count!=3U)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_search_session_model_cancel(&session)!=UMI_STATUS_OK||session.phase!=UMI_EDITOR_INTEL_PHASE_CANCELLED)return 4;return 0;}
