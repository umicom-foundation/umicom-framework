/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/refactor_checkpoint.c
 *
 * PURPOSE:
 *   Model refactor checkpoint as toolkit-neutral Framework-owned editor intelligence state.
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
#include "umicom/editor/intelligence_workbench/refactor_checkpoint.h"

#include <string.h>

/*
 * Provide the editor intel refactor checkpoint begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_intel_refactor_checkpoint_begin(UmiEditorIntelRefactorCheckpoint *transaction,const char *transaction_id,uint32_t total_operations){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction==NULL||!umi_editor_intel_id_valid(transaction_id))return UMI_STATUS_INVALID_ARGUMENT;memset(transaction,0,sizeof *transaction);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_copy_text(transaction->transaction_id,sizeof transaction->transaction_id,transaction_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;transaction->phase=UMI_EDITOR_INTEL_PHASE_READY;transaction->total_operations=total_operations;transaction->revision=1U;return UMI_STATUS_OK;}
/*
 * Perform editor intel refactor checkpoint record through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_editor_intel_refactor_checkpoint_record_apply(UmiEditorIntelRefactorCheckpoint *transaction){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction==NULL||(transaction->phase!=UMI_EDITOR_INTEL_PHASE_READY&&transaction->phase!=UMI_EDITOR_INTEL_PHASE_APPLYING))return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction->applied_operations>=transaction->total_operations)return UMI_STATUS_CAPACITY_EXCEEDED;transaction->phase=UMI_EDITOR_INTEL_PHASE_APPLYING;transaction->applied_operations++;transaction->revision++;return UMI_STATUS_OK;}
/*
 * Provide the editor intel refactor checkpoint commit operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_checkpoint_commit(UmiEditorIntelRefactorCheckpoint *transaction){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction==NULL||transaction->applied_operations!=transaction->total_operations)return UMI_STATUS_INVALID_STATE;transaction->phase=UMI_EDITOR_INTEL_PHASE_COMMITTED;transaction->revision++;return UMI_STATUS_OK;}
/*
 * Provide the editor intel refactor checkpoint rollback operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_checkpoint_rollback(UmiEditorIntelRefactorCheckpoint *transaction){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction==NULL||transaction->phase==UMI_EDITOR_INTEL_PHASE_COMMITTED)return UMI_STATUS_INVALID_STATE;transaction->phase=UMI_EDITOR_INTEL_PHASE_ROLLED_BACK;transaction->revision++;return UMI_STATUS_OK;}
/*
 * Check that editor intel refactor checkpoint satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_checkpoint_valid(const UmiEditorIntelRefactorCheckpoint *transaction){return transaction!=NULL&&umi_editor_intel_id_valid(transaction->transaction_id)&&transaction->applied_operations<=transaction->total_operations&&transaction->phase>=UMI_EDITOR_INTEL_PHASE_READY&&transaction->phase<=UMI_EDITOR_INTEL_PHASE_ROLLED_BACK;}
