/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_refactor_restore.c
 *
 * PURPOSE:
 *   Implement the test refactor restore behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/refactor_restore.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEditorIntelRefactorRestore session;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_restore_begin(&session,"session-1")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_restore_set_ready(&session,3U)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_intel_refactor_restore_valid(&session)||session.item_count!=3U)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_restore_cancel(&session)!=UMI_STATUS_OK||session.phase!=UMI_EDITOR_INTEL_PHASE_CANCELLED)return 4;return 0;}
