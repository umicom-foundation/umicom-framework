/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_refactor_transaction.c
 *
 * PURPOSE:
 *   Implement the test refactor transaction behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/refactor_transaction.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEditorIntelRefactorTransaction transaction;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_transaction_begin(&transaction,"tx-1",2U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_transaction_record_apply(&transaction)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_transaction_record_apply(&transaction)!=UMI_STATUS_OK)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_transaction_commit(&transaction)!=UMI_STATUS_OK)return 4;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_intel_refactor_transaction_valid(&transaction)||transaction.phase!=UMI_EDITOR_INTEL_PHASE_COMMITTED)return 5;return 0;}
