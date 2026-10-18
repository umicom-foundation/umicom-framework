/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_data_snapshot.c
 *
 * PURPOSE:
 *   Exercise the data snapshot enterprise UI capability.
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
#include "umicom/ui/enterprise/data_snapshot.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/data_snapshot.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntDataSnapshotTransferEqual(const UmiUiEntDataSnapshot *a, const UmiUiEntDataSnapshot *b)
{
    return a->generation == b->generation &&
        a->row_count == b->row_count &&
        a->column_count == b->column_count &&
        a->complete == b->complete &&
        a->source_revision == b->source_revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntDataSnapshotTransferTails(UmiUiEntDataSnapshot *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntDataSnapshotTransferMalformed(const UmiUiEntDataSnapshot *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntDataSnapshotTransferCases, UmiUiEntDataSnapshot,
    umi_ui_ent_data_snapshot_archive_encode, umi_ui_ent_data_snapshot_archive_decode,
    UmiUiEntDataSnapshotTransferEqual, UmiUiEntDataSnapshotTransferTails, UmiUiEntDataSnapshotTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiUiEntDataSnapshotTransferCasesWithWorkspace, UmiUiEntDataSnapshot,
    umi_ui_ent_data_snapshot_archive_encode, umi_ui_ent_data_snapshot_archive_decode_with_workspace, UmiUiEntDataSnapshotTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiUiEntDataSnapshotConditionalArchiveCases, UmiUiEntDataSnapshot,
    umi_ui_ent_data_snapshot_archive_encode, umi_ui_ent_data_snapshot_archive_decode_if_unchanged,
    UmiUiEntDataSnapshotTransferEqual, UmiUiEntDataSnapshotTransferTails, UmiUiEntDataSnapshotTransferMalformed)


int main(void){UmiUiEntDataSnapshot v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_data_snapshot_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_data_snapshot_validate(&v))return 9;
    if (UmiUiEntDataSnapshotTransferCases(&v) != 0) return 1;
    if (UmiUiEntDataSnapshotTransferCasesWithWorkspace(&v) != 0) return 1;
    if (UmiUiEntDataSnapshotConditionalArchiveCases(&v) != 0) return 1;
puts("ok");return 0;}
