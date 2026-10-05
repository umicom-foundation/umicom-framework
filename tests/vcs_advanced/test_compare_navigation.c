/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_compare_navigation.c
 *
 * PURPOSE:
 *   Validate track deterministic next/previous change navigation in comparison sessions.
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
#include "umicom/vcs/advanced/compare_navigation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/vcs/advanced/compare_navigation.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiVcsAdvancedCompareNavigationTransferEqual(const UmiVcsAdvancedCompareNavigation *a, const UmiVcsAdvancedCompareNavigation *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        a->change_count == b->change_count &&
        a->current_index == b->current_index &&
        a->wrap == b->wrap;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiVcsAdvancedCompareNavigationTransferTails(UmiVcsAdvancedCompareNavigation *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiVcsAdvancedCompareNavigationTransferMalformed(const UmiVcsAdvancedCompareNavigation *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiVcsAdvancedCompareNavigationTransferCases, UmiVcsAdvancedCompareNavigation,
    umi_vcs_advanced_compare_navigation_archive_encode, umi_vcs_advanced_compare_navigation_archive_decode,
    UmiVcsAdvancedCompareNavigationTransferEqual, UmiVcsAdvancedCompareNavigationTransferTails, UmiVcsAdvancedCompareNavigationTransferMalformed)

int main(void)
{
    UmiVcsAdvancedCompareNavigation value;
    umi_vcs_advanced_compare_navigation_init(&value);
    value.change_count = 3U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_compare_navigation_validate(&value) != UMI_STATUS_OK) return 2;
    if (UmiVcsAdvancedCompareNavigationTransferCases(&value) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_compare_navigation_next(&value) || value.current_index != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_compare_navigation_previous(&value) || value.current_index != 0U) return 4;
    return 0;
}
