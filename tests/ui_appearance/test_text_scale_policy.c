/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_text_scale_policy.c
 *
 * PURPOSE:
 *   Verify clamp user text scaling while preserving semantic size hierarchy and accessibility intent.
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
#include "umicom/ui/appearance/text_scale_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/text_scale_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceTextScalePolicyTransferEqual(const UmiAppearanceTextScalePolicy *a, const UmiAppearanceTextScalePolicy *b)
{
    return strcmp(a->policy_id, b->policy_id) == 0 &&
        a->minimum_scale == b->minimum_scale &&
        a->maximum_scale == b->maximum_scale &&
        a->requested_scale == b->requested_scale &&
        a->resolved_scale == b->resolved_scale;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceTextScalePolicyTransferTails(UmiAppearanceTextScalePolicy *value)
{
    (void)value;
    {
        size_t used = strlen(value->policy_id) + 1U;
        memset(value->policy_id + used, 0xa5, sizeof(value->policy_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceTextScalePolicyTransferMalformed(const UmiAppearanceTextScalePolicy *sample)
{
    (void)sample;
    {
        UmiAppearanceTextScalePolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.policy_id, 'x', sizeof(invalid.policy_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_text_scale_policy_is_valid(&invalid)) ||
            umi_appearance_text_scale_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated policy_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceTextScalePolicyTransferCases, UmiAppearanceTextScalePolicy,
    umi_appearance_text_scale_policy_archive_encode, umi_appearance_text_scale_policy_archive_decode,
    UmiAppearanceTextScalePolicyTransferEqual, UmiAppearanceTextScalePolicyTransferTails, UmiAppearanceTextScalePolicyTransferMalformed)

int main(void) {
    UmiAppearanceTextScalePolicy item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_text_scale_policy_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_text_scale_policy_is_valid(&item)) return 2;
    if (UmiAppearanceTextScalePolicyTransferCases(&item) != 0) return 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_text_scale_policy_resolve(&item,5.0)!=UMI_STATUS_OK || item.resolved_scale!=3.0) return 3;
    return 0;
}
