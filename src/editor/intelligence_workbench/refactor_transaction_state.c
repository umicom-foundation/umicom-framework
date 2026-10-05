/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/refactor_transaction_state.c
 *
 * PURPOSE:
 *   Model refactor transaction state as toolkit-neutral Framework-owned editor intelligence state.
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
#include "umicom/editor/intelligence_workbench/refactor_transaction_state.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Provide the editor intel refactor transaction state begin operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_transaction_state_begin(UmiEditorIntelRefactorTransactionState *transaction,const char *transaction_id,uint32_t total_operations){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction==NULL||!umi_editor_intel_id_valid(transaction_id))return UMI_STATUS_INVALID_ARGUMENT;memset(transaction,0,sizeof *transaction);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_copy_text(transaction->transaction_id,sizeof transaction->transaction_id,transaction_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;transaction->phase=UMI_EDITOR_INTEL_PHASE_READY;transaction->total_operations=total_operations;transaction->revision=1U;return UMI_STATUS_OK;}
/*
 * Perform editor intel refactor transaction state record through the module contract so
 * client applications do not duplicate its policy.
 */
UmiStatus umi_editor_intel_refactor_transaction_state_record_apply(UmiEditorIntelRefactorTransactionState *transaction){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction==NULL||(transaction->phase!=UMI_EDITOR_INTEL_PHASE_READY&&transaction->phase!=UMI_EDITOR_INTEL_PHASE_APPLYING))return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction->applied_operations>=transaction->total_operations)return UMI_STATUS_CAPACITY_EXCEEDED;transaction->phase=UMI_EDITOR_INTEL_PHASE_APPLYING;transaction->applied_operations++;transaction->revision++;return UMI_STATUS_OK;}
/*
 * Provide the editor intel refactor transaction state commit operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_transaction_state_commit(UmiEditorIntelRefactorTransactionState *transaction){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction==NULL||transaction->applied_operations!=transaction->total_operations)return UMI_STATUS_INVALID_STATE;transaction->phase=UMI_EDITOR_INTEL_PHASE_COMMITTED;transaction->revision++;return UMI_STATUS_OK;}
/*
 * Provide the editor intel refactor transaction state rollback operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_refactor_transaction_state_rollback(UmiEditorIntelRefactorTransactionState *transaction){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction==NULL||transaction->phase==UMI_EDITOR_INTEL_PHASE_COMMITTED)return UMI_STATUS_INVALID_STATE;transaction->phase=UMI_EDITOR_INTEL_PHASE_ROLLED_BACK;transaction->revision++;return UMI_STATUS_OK;}
/*
 * Check that editor intel refactor transaction state satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_transaction_state_valid(const UmiEditorIntelRefactorTransactionState *transaction){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (transaction == NULL) return 0;
    if (memchr(transaction->transaction_id, '\0', sizeof(transaction->transaction_id)) == NULL) return 0;
return transaction!=NULL&&umi_editor_intel_id_valid(transaction->transaction_id)&&transaction->applied_operations<=transaction->total_operations&&transaction->phase>=UMI_EDITOR_INTEL_PHASE_READY&&transaction->phase<=UMI_EDITOR_INTEL_PHASE_ROLLED_BACK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorIntelRefactorTransactionStateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4f53b1762cb818d7);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorIntelRefactorTransactionState *)0)->transaction_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorIntelRefactorTransactionStateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorIntelRefactorTransactionState *)0)->transaction_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiEditorIntelRefactorTransactionStateArchiveWrite(UmiArchiveWriter *writer, const UmiEditorIntelRefactorTransactionState *value)
{
    UmiArchiveWriteText(writer, value->transaction_id, sizeof(value->transaction_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->phase);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->total_operations);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->applied_operations);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiEditorIntelRefactorTransactionStateArchiveRead(UmiArchiveReader *reader, UmiEditorIntelRefactorTransactionState *value)
{
    UmiArchiveReadText(reader, value->transaction_id, sizeof(value->transaction_id));
    value->phase = (UmiEditorIntelPhase)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->total_operations = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->applied_operations = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiEditorIntelRefactorTransactionStateArchiveValidate(const UmiEditorIntelRefactorTransactionState *value)
{
    return umi_editor_intel_refactor_transaction_state_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_intel_refactor_transaction_state_archive_encode, umi_editor_intel_refactor_transaction_state_archive_decode,
    UmiEditorIntelRefactorTransactionState, UmiEditorIntelRefactorTransactionStateArchiveSchema, UmiEditorIntelRefactorTransactionStateArchiveBound, UmiEditorIntelRefactorTransactionStateArchiveWrite, UmiEditorIntelRefactorTransactionStateArchiveRead, UmiEditorIntelRefactorTransactionStateArchiveValidate)
