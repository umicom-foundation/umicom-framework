/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_session_snapshot.c
 *
 * PURPOSE:
 *   Implement the test session snapshot behavior for
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
#include "umicom/editor/workbench/session_snapshot.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/workbench/session_snapshot.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorWbSessionSnapshotTransferEqual(const UmiEditorWbSessionSnapshot *a, const UmiEditorWbSessionSnapshot *b)
{
    return strcmp(a->active_id, b->active_id) == 0 &&
        a->item_count == b->item_count &&
        a->group_count == b->group_count &&
        a->revision == b->revision &&
        a->fingerprint == b->fingerprint;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorWbSessionSnapshotTransferTails(UmiEditorWbSessionSnapshot *value)
{
    (void)value;
    {
        size_t used = strlen(value->active_id) + 1U;
        memset(value->active_id + used, 0xa5, sizeof(value->active_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorWbSessionSnapshotTransferMalformed(const UmiEditorWbSessionSnapshot *sample)
{
    (void)sample;
    {
        UmiEditorWbSessionSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.active_id, 'x', sizeof(invalid.active_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_session_snapshot_valid(&invalid)) ||
            umi_editor_wb_session_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated active_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorWbSessionSnapshotTransferCases, UmiEditorWbSessionSnapshot,
    umi_editor_wb_session_snapshot_archive_encode, umi_editor_wb_session_snapshot_archive_decode,
    UmiEditorWbSessionSnapshotTransferEqual, UmiEditorWbSessionSnapshotTransferTails, UmiEditorWbSessionSnapshotTransferMalformed)

int main(void){ UmiEditorWbSessionSnapshot s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_session_snapshot_capture(&s,"active",5U,2U,9U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_session_snapshot_valid(&s)||s.fingerprint==0U)return 2;
    if (UmiEditorWbSessionSnapshotTransferCases(&s) != 0) return 1;
 return 0; }
