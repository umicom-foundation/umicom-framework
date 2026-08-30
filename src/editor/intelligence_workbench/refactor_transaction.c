/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/refactor_transaction.c
 *
 * PURPOSE:
 *   Coordinate begin, apply, commit and rollback state for one refactor.
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
#include "umicom/editor/intelligence_workbench/refactor_transaction.h"

#include <string.h>

UmiStatus umi_editor_intel_refactor_transaction_begin(UmiEditorIntelRefactorTransaction *transaction,const char *transaction_id,uint32_t total_operations){if(transaction==NULL||!umi_editor_intel_id_valid(transaction_id))return UMI_STATUS_INVALID_ARGUMENT;memset(transaction,0,sizeof *transaction);if(umi_editor_intel_copy_text(transaction->transaction_id,sizeof transaction->transaction_id,transaction_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;transaction->phase=UMI_EDITOR_INTEL_PHASE_READY;transaction->total_operations=total_operations;transaction->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_editor_intel_refactor_transaction_record_apply(UmiEditorIntelRefactorTransaction *transaction){if(transaction==NULL||(transaction->phase!=UMI_EDITOR_INTEL_PHASE_READY&&transaction->phase!=UMI_EDITOR_INTEL_PHASE_APPLYING))return UMI_STATUS_INVALID_STATE;if(transaction->applied_operations>=transaction->total_operations)return UMI_STATUS_CAPACITY_EXCEEDED;transaction->phase=UMI_EDITOR_INTEL_PHASE_APPLYING;transaction->applied_operations++;transaction->revision++;return UMI_STATUS_OK;}
UmiStatus umi_editor_intel_refactor_transaction_commit(UmiEditorIntelRefactorTransaction *transaction){if(transaction==NULL||transaction->applied_operations!=transaction->total_operations)return UMI_STATUS_INVALID_STATE;transaction->phase=UMI_EDITOR_INTEL_PHASE_COMMITTED;transaction->revision++;return UMI_STATUS_OK;}
UmiStatus umi_editor_intel_refactor_transaction_rollback(UmiEditorIntelRefactorTransaction *transaction){if(transaction==NULL||transaction->phase==UMI_EDITOR_INTEL_PHASE_COMMITTED)return UMI_STATUS_INVALID_STATE;transaction->phase=UMI_EDITOR_INTEL_PHASE_ROLLED_BACK;transaction->revision++;return UMI_STATUS_OK;}
int umi_editor_intel_refactor_transaction_valid(const UmiEditorIntelRefactorTransaction *transaction){return transaction!=NULL&&umi_editor_intel_id_valid(transaction->transaction_id)&&transaction->applied_operations<=transaction->total_operations&&transaction->phase>=UMI_EDITOR_INTEL_PHASE_READY&&transaction->phase<=UMI_EDITOR_INTEL_PHASE_ROLLED_BACK;}
