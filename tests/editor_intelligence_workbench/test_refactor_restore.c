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
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/editor/intelligence_workbench/refactor_restore.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/intelligence_workbench/refactor_restore.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorIntelRefactorRestoreTransferEqual(const UmiEditorIntelRefactorRestore *a, const UmiEditorIntelRefactorRestore *b)
{
    return strcmp(a->session_id, b->session_id) == 0 &&
        a->phase == b->phase &&
        a->item_count == b->item_count &&
        a->changed == b->changed &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorIntelRefactorRestoreTransferTails(UmiEditorIntelRefactorRestore *value)
{
    (void)value;
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorIntelRefactorRestoreTransferMalformed(const UmiEditorIntelRefactorRestore *sample)
{
    (void)sample;
    {
        UmiEditorIntelRefactorRestore invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.session_id, 'x', sizeof(invalid.session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_intel_refactor_restore_valid(&invalid)) ||
            umi_editor_intel_refactor_restore_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated session_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorIntelRefactorRestoreTransferCases, UmiEditorIntelRefactorRestore,
    umi_editor_intel_refactor_restore_archive_encode, umi_editor_intel_refactor_restore_archive_decode,
    UmiEditorIntelRefactorRestoreTransferEqual, UmiEditorIntelRefactorRestoreTransferTails, UmiEditorIntelRefactorRestoreTransferMalformed)

int main(void){UmiEditorIntelRefactorRestore session;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_restore_begin(&session,"session-1")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_restore_set_ready(&session,3U)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_intel_refactor_restore_valid(&session)||session.item_count!=3U)return 3;
    if (UmiEditorIntelRefactorRestoreTransferCases(&session) != 0) return 1;
/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_refactor_restore_cancel(&session)!=UMI_STATUS_OK||session.phase!=UMI_EDITOR_INTEL_PHASE_CANCELLED)return 4;return 0;}
