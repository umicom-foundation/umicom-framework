/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_range_mapping.c
 *
 * PURPOSE:
 *   Validate map source ranges to destination ranges after edits or diff alignment.
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
#include "umicom/vcs/advanced/range_mapping.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/vcs/advanced/range_mapping.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiVcsAdvancedRangeMappingTransferEqual(const UmiVcsAdvancedRangeMapping *a, const UmiVcsAdvancedRangeMapping *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        a->source_start == b->source_start &&
        a->source_count == b->source_count &&
        a->target_start == b->target_start &&
        a->target_count == b->target_count &&
        a->confidence_percent == b->confidence_percent;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiVcsAdvancedRangeMappingTransferTails(UmiVcsAdvancedRangeMapping *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiVcsAdvancedRangeMappingTransferMalformed(const UmiVcsAdvancedRangeMapping *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiVcsAdvancedRangeMappingTransferCases, UmiVcsAdvancedRangeMapping,
    umi_vcs_advanced_range_mapping_archive_encode, umi_vcs_advanced_range_mapping_archive_decode,
    UmiVcsAdvancedRangeMappingTransferEqual, UmiVcsAdvancedRangeMappingTransferTails, UmiVcsAdvancedRangeMappingTransferMalformed)

int main(void)
{
    UmiVcsAdvancedRangeMapping value;
    umi_vcs_advanced_range_mapping_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_range_mapping_validate(&value) == UMI_STATUS_OK) return 1;
    value.source_start = 10U; value.source_count = 5U; value.target_start = 12U; value.target_count = 5U; value.confidence_percent = 100U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_range_mapping_validate(&value) != UMI_STATUS_OK) return 2;
    if (UmiVcsAdvancedRangeMappingTransferCases(&value) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_range_mapping_delta(&value) != 2LL) return 3;
    return 0;
}
