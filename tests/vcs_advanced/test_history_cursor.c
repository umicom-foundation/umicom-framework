/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_history_cursor.c
 *
 * PURPOSE:
 *   Validate track deterministic pagination through large repository histories.
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
#include "umicom/vcs/advanced/history_cursor.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/vcs/advanced/history_cursor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiVcsAdvancedHistoryCursorTransferEqual(const UmiVcsAdvancedHistoryCursor *a, const UmiVcsAdvancedHistoryCursor *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        a->offset == b->offset &&
        a->limit == b->limit &&
        a->returned == b->returned &&
        a->total_hint == b->total_hint &&
        a->has_more == b->has_more;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiVcsAdvancedHistoryCursorTransferTails(UmiVcsAdvancedHistoryCursor *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiVcsAdvancedHistoryCursorTransferMalformed(const UmiVcsAdvancedHistoryCursor *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiVcsAdvancedHistoryCursorTransferCases, UmiVcsAdvancedHistoryCursor,
    umi_vcs_advanced_history_cursor_archive_encode, umi_vcs_advanced_history_cursor_archive_decode,
    UmiVcsAdvancedHistoryCursorTransferEqual, UmiVcsAdvancedHistoryCursorTransferTails, UmiVcsAdvancedHistoryCursorTransferMalformed)

int main(void)
{
    UmiVcsAdvancedHistoryCursor value;
    umi_vcs_advanced_history_cursor_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_history_cursor_validate(&value) != UMI_STATUS_OK) return 2;
    if (UmiVcsAdvancedHistoryCursorTransferCases(&value) != 0) return 1;

    umi_vcs_advanced_history_cursor_advance(&value, 25U, 1);
    /* Apply this branch only when its contract condition is satisfied. */
    if (value.offset != 25U || value.has_more == 0) return 3;
    return 0;
}
