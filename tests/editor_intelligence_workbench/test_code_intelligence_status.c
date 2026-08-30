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
int main(void){UmiEditorIntelCodeIntelligenceStatus session;if(umi_editor_intel_code_intelligence_status_begin(&session,"session-1")!=UMI_STATUS_OK)return 1;if(umi_editor_intel_code_intelligence_status_set_ready(&session,3U)!=UMI_STATUS_OK)return 2;if(!umi_editor_intel_code_intelligence_status_valid(&session)||session.item_count!=3U)return 3;if(umi_editor_intel_code_intelligence_status_cancel(&session)!=UMI_STATUS_OK||session.phase!=UMI_EDITOR_INTEL_PHASE_CANCELLED)return 4;return 0;}
