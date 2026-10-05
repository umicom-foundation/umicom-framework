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
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/editor/intelligence_workbench/refactor_rollback_report.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/intelligence_workbench/refactor_rollback_report.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorIntelRefactorRollbackReportTransferEqual(const UmiEditorIntelRefactorRollbackReport *a, const UmiEditorIntelRefactorRollbackReport *b)
{
    return strcmp(a->transaction_id, b->transaction_id) == 0 &&
        a->phase == b->phase &&
        a->total_operations == b->total_operations &&
        a->applied_operations == b->applied_operations &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorIntelRefactorRollbackReportTransferTails(UmiEditorIntelRefactorRollbackReport *value)
{
    (void)value;
    {
        size_t used = strlen(value->transaction_id) + 1U;
        memset(value->transaction_id + used, 0xa5, sizeof(value->transaction_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorIntelRefactorRollbackReportTransferMalformed(const UmiEditorIntelRefactorRollbackReport *sample)
{
    (void)sample;
    {
        UmiEditorIntelRefactorRollbackReport invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.transaction_id, 'x', sizeof(invalid.transaction_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_intel_refactor_rollback_report_valid(&invalid)) ||
            umi_editor_intel_refactor_rollback_report_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated transaction_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorIntelRefactorRollbackReportTransferCases, UmiEditorIntelRefactorRollbackReport,
    umi_editor_intel_refactor_rollback_report_archive_encode, umi_editor_intel_refactor_rollback_report_archive_decode,
    UmiEditorIntelRefactorRollbackReportTransferEqual, UmiEditorIntelRefactorRollbackReportTransferTails, UmiEditorIntelRefactorRollbackReportTransferMalformed)

int main(void){UmiEditorIntelRefactorRollbackReport transaction;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_rollback_report_begin(&transaction,"tx-1",2U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_rollback_report_record_apply(&transaction)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_rollback_report_record_apply(&transaction)!=UMI_STATUS_OK)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_rollback_report_commit(&transaction)!=UMI_STATUS_OK)return 4;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_intel_refactor_rollback_report_valid(&transaction)||transaction.phase!=UMI_EDITOR_INTEL_PHASE_COMMITTED)return 5;
    if (UmiEditorIntelRefactorRollbackReportTransferCases(&transaction) != 0) return 1;
return 0;}
