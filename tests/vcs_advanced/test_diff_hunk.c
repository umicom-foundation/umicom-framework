/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_diff_hunk.c
 *
 * PURPOSE:
 *   Validate describe normalized change blocks for navigation and partial operations.
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
#include "umicom/vcs/advanced/diff_hunk.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/vcs/advanced/diff_hunk.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiVcsAdvancedDiffHunkTransferEqual(const UmiVcsAdvancedDiffHunk *a, const UmiVcsAdvancedDiffHunk *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        a->old_start == b->old_start &&
        a->old_count == b->old_count &&
        a->new_start == b->new_start &&
        a->new_count == b->new_count &&
        a->added_lines == b->added_lines &&
        a->deleted_lines == b->deleted_lines &&
        a->modified_lines == b->modified_lines &&
        a->fingerprint == b->fingerprint;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiVcsAdvancedDiffHunkTransferTails(UmiVcsAdvancedDiffHunk *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiVcsAdvancedDiffHunkTransferMalformed(const UmiVcsAdvancedDiffHunk *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiVcsAdvancedDiffHunkTransferCases, UmiVcsAdvancedDiffHunk,
    umi_vcs_advanced_diff_hunk_archive_encode, umi_vcs_advanced_diff_hunk_archive_decode,
    UmiVcsAdvancedDiffHunkTransferEqual, UmiVcsAdvancedDiffHunkTransferTails, UmiVcsAdvancedDiffHunkTransferMalformed)

int main(void)
{
    UmiVcsAdvancedDiffHunk value;
    umi_vcs_advanced_diff_hunk_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_diff_hunk_validate(&value) == UMI_STATUS_OK) return 1;
    value.old_start = 1U; value.old_count = 3U; value.new_start = 1U; value.new_count = 4U;
    umi_vcs_advanced_diff_hunk_set_counts(&value, 1U, 0U, 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_diff_hunk_validate(&value) != UMI_STATUS_OK) return 2;
    if (UmiVcsAdvancedDiffHunkTransferCases(&value) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_diff_hunk_change_count(&value) != 2U) return 3;
    return 0;
}
