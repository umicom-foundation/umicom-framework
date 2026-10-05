/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_moved_block.c
 *
 * PURPOSE:
 *   Validate capture identical or near-identical blocks moved within a document.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
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
#include "umicom/vcs/advanced/moved_block.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/vcs/advanced/moved_block.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiVcsAdvancedMovedBlockTransferEqual(const UmiVcsAdvancedMovedBlock *a, const UmiVcsAdvancedMovedBlock *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        a->old_start == b->old_start &&
        a->new_start == b->new_start &&
        a->line_count == b->line_count &&
        a->fingerprint == b->fingerprint &&
        a->confidence_percent == b->confidence_percent;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiVcsAdvancedMovedBlockTransferTails(UmiVcsAdvancedMovedBlock *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiVcsAdvancedMovedBlockTransferMalformed(const UmiVcsAdvancedMovedBlock *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiVcsAdvancedMovedBlockTransferCases, UmiVcsAdvancedMovedBlock,
    umi_vcs_advanced_moved_block_archive_encode, umi_vcs_advanced_moved_block_archive_decode,
    UmiVcsAdvancedMovedBlockTransferEqual, UmiVcsAdvancedMovedBlockTransferTails, UmiVcsAdvancedMovedBlockTransferMalformed)

int main(void)
{
    UmiVcsAdvancedMovedBlock value;
    umi_vcs_advanced_moved_block_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_moved_block_validate(&value) == UMI_STATUS_OK) return 1;
    value.old_start = 1U; value.new_start = 20U; value.line_count = 5U; value.confidence_percent = 100U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_moved_block_validate(&value) != UMI_STATUS_OK) return 2;
    if (UmiVcsAdvancedMovedBlockTransferCases(&value) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_moved_block_is_significant(&value, 3U, 90U)) return 3;
    return 0;
}
