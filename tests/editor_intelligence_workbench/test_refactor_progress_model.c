/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_refactor_progress_model.c
 *
 * PURPOSE:
 *   Implement the test refactor progress model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/refactor_progress_model.h"
int main(void){UmiEditorIntelRefactorProgressModel session;if(umi_editor_intel_refactor_progress_model_begin(&session,"session-1")!=UMI_STATUS_OK)return 1;if(umi_editor_intel_refactor_progress_model_set_ready(&session,3U)!=UMI_STATUS_OK)return 2;if(!umi_editor_intel_refactor_progress_model_valid(&session)||session.item_count!=3U)return 3;if(umi_editor_intel_refactor_progress_model_cancel(&session)!=UMI_STATUS_OK||session.phase!=UMI_EDITOR_INTEL_PHASE_CANCELLED)return 4;return 0;}
