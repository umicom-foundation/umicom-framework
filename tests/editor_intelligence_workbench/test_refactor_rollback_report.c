/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_refactor_rollback_report.c
 *
 * PURPOSE:
 *   Implement the test refactor rollback report behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/refactor_rollback_report.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEditorIntelRefactorRollbackReport transaction;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_rollback_report_begin(&transaction,"tx-1",2U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_rollback_report_record_apply(&transaction)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_rollback_report_record_apply(&transaction)!=UMI_STATUS_OK)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_rollback_report_commit(&transaction)!=UMI_STATUS_OK)return 4;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_intel_refactor_rollback_report_valid(&transaction)||transaction.phase!=UMI_EDITOR_INTEL_PHASE_COMMITTED)return 5;return 0;}
