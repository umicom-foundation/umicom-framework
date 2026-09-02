/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_code_intelligence_status.c
 *
 * PURPOSE:
 *   Implement the test code intelligence status behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/code_intelligence_status.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEditorIntelCodeIntelligenceStatus session;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_code_intelligence_status_begin(&session,"session-1")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_code_intelligence_status_set_ready(&session,3U)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_intel_code_intelligence_status_valid(&session)||session.item_count!=3U)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_code_intelligence_status_cancel(&session)!=UMI_STATUS_OK||session.phase!=UMI_EDITOR_INTEL_PHASE_CANCELLED)return 4;return 0;}
