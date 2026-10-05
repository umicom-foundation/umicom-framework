/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_typography_policy.c
 *
 * PURPOSE:
 *   Verify govern semantic typography scaling, minimum readable text size and font smoothing intent.
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
#include "umicom/ui/appearance/typography_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/typography_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceTypographyPolicyTransferEqual(const UmiAppearanceTypographyPolicy *a, const UmiAppearanceTypographyPolicy *b)
{
    return strcmp(a->policy_id, b->policy_id) == 0 &&
        a->base_text_scale == b->base_text_scale &&
        a->minimum_text_dp == b->minimum_text_dp &&
        a->maximum_text_scale == b->maximum_text_scale &&
        a->respect_user_scale == b->respect_user_scale;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceTypographyPolicyTransferTails(UmiAppearanceTypographyPolicy *value)
{
    (void)value;
    {
        size_t used = strlen(value->policy_id) + 1U;
        memset(value->policy_id + used, 0xa5, sizeof(value->policy_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceTypographyPolicyTransferMalformed(const UmiAppearanceTypographyPolicy *sample)
{
    (void)sample;
    {
        UmiAppearanceTypographyPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.policy_id, 'x', sizeof(invalid.policy_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_typography_policy_is_valid(&invalid)) ||
            umi_appearance_typography_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated policy_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceTypographyPolicyTransferCases, UmiAppearanceTypographyPolicy,
    umi_appearance_typography_policy_archive_encode, umi_appearance_typography_policy_archive_decode,
    UmiAppearanceTypographyPolicyTransferEqual, UmiAppearanceTypographyPolicyTransferTails, UmiAppearanceTypographyPolicyTransferMalformed)

int main(void) {
    UmiAppearanceTypographyPolicy item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_typography_policy_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_typography_policy_is_valid(&item)) return 2;
    if (UmiAppearanceTypographyPolicyTransferCases(&item) != 0) return 1;

    return 0;
}
