/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/refactor_rollback_report.c
 *
 * PURPOSE:
 *   Model refactor rollback report as toolkit-neutral Framework-owned editor intelligence state.
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
#include "umicom/editor/intelligence_workbench/refactor_rollback_report.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Provide the editor intel refactor rollback report begin operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_rollback_report_begin(UmiEditorIntelRefactorRollbackReport *transaction,const char *transaction_id,uint32_t total_operations){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction==NULL||!umi_editor_intel_id_valid(transaction_id))return UMI_STATUS_INVALID_ARGUMENT;memset(transaction,0,sizeof *transaction);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_copy_text(transaction->transaction_id,sizeof transaction->transaction_id,transaction_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;transaction->phase=UMI_EDITOR_INTEL_PHASE_READY;transaction->total_operations=total_operations;transaction->revision=1U;return UMI_STATUS_OK;}
/*
 * Perform editor intel refactor rollback report record through the module contract so
 * client applications do not duplicate its policy.
 */
UmiStatus umi_editor_intel_refactor_rollback_report_record_apply(UmiEditorIntelRefactorRollbackReport *transaction){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction==NULL||(transaction->phase!=UMI_EDITOR_INTEL_PHASE_READY&&transaction->phase!=UMI_EDITOR_INTEL_PHASE_APPLYING))return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction->applied_operations>=transaction->total_operations)return UMI_STATUS_CAPACITY_EXCEEDED;transaction->phase=UMI_EDITOR_INTEL_PHASE_APPLYING;transaction->applied_operations++;transaction->revision++;return UMI_STATUS_OK;}
/*
 * Provide the editor intel refactor rollback report commit operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_rollback_report_commit(UmiEditorIntelRefactorRollbackReport *transaction){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction==NULL||transaction->applied_operations!=transaction->total_operations)return UMI_STATUS_INVALID_STATE;transaction->phase=UMI_EDITOR_INTEL_PHASE_COMMITTED;transaction->revision++;return UMI_STATUS_OK;}
/*
 * Provide the editor intel refactor rollback report rollback operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_rollback_report_rollback(UmiEditorIntelRefactorRollbackReport *transaction){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(transaction==NULL||transaction->phase==UMI_EDITOR_INTEL_PHASE_COMMITTED)return UMI_STATUS_INVALID_STATE;transaction->phase=UMI_EDITOR_INTEL_PHASE_ROLLED_BACK;transaction->revision++;return UMI_STATUS_OK;}
/*
 * Check that editor intel refactor rollback report satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_rollback_report_valid(const UmiEditorIntelRefactorRollbackReport *transaction){
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
static uint64_t UmiEditorIntelRefactorRollbackReportArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfac5463f769e76ac);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorIntelRefactorRollbackReport *)0)->transaction_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorIntelRefactorRollbackReportArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorIntelRefactorRollbackReport *)0)->transaction_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiEditorIntelRefactorRollbackReportArchiveWrite(UmiArchiveWriter *writer, const UmiEditorIntelRefactorRollbackReport *value)
{
    UmiArchiveWriteText(writer, value->transaction_id, sizeof(value->transaction_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->phase);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->total_operations);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->applied_operations);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiEditorIntelRefactorRollbackReportArchiveRead(UmiArchiveReader *reader, UmiEditorIntelRefactorRollbackReport *value)
{
    UmiArchiveReadText(reader, value->transaction_id, sizeof(value->transaction_id));
    value->phase = (UmiEditorIntelPhase)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->total_operations = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->applied_operations = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiEditorIntelRefactorRollbackReportArchiveValidate(const UmiEditorIntelRefactorRollbackReport *value)
{
    return umi_editor_intel_refactor_rollback_report_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_intel_refactor_rollback_report_archive_encode, umi_editor_intel_refactor_rollback_report_archive_decode,
    UmiEditorIntelRefactorRollbackReport, UmiEditorIntelRefactorRollbackReportArchiveSchema, UmiEditorIntelRefactorRollbackReportArchiveBound, UmiEditorIntelRefactorRollbackReportArchiveWrite, UmiEditorIntelRefactorRollbackReportArchiveRead, UmiEditorIntelRefactorRollbackReportArchiveValidate)
