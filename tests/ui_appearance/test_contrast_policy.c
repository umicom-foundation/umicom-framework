/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_contrast_policy.c
 *
 * PURPOSE:
 *   Verify define certification thresholds for normal text, large text, icons and focus indicators.
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
#include "umicom/ui/appearance/contrast_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/contrast_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceContrastPolicyTransferEqual(const UmiAppearanceContrastPolicy *a, const UmiAppearanceContrastPolicy *b)
{
    return strcmp(a->policy_id, b->policy_id) == 0 &&
        a->normal_text_ratio == b->normal_text_ratio &&
        a->large_text_ratio == b->large_text_ratio &&
        a->non_text_ratio == b->non_text_ratio &&
        a->focus_ratio == b->focus_ratio;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceContrastPolicyTransferTails(UmiAppearanceContrastPolicy *value)
{
    (void)value;
    {
        size_t used = strlen(value->policy_id) + 1U;
        memset(value->policy_id + used, 0xa5, sizeof(value->policy_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceContrastPolicyTransferMalformed(const UmiAppearanceContrastPolicy *sample)
{
    (void)sample;
    {
        UmiAppearanceContrastPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.policy_id, 'x', sizeof(invalid.policy_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_contrast_policy_is_valid(&invalid)) ||
            umi_appearance_contrast_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated policy_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceContrastPolicyTransferCases, UmiAppearanceContrastPolicy,
    umi_appearance_contrast_policy_archive_encode, umi_appearance_contrast_policy_archive_decode,
    UmiAppearanceContrastPolicyTransferEqual, UmiAppearanceContrastPolicyTransferTails, UmiAppearanceContrastPolicyTransferMalformed)

int main(void) {
    UmiAppearanceContrastPolicy item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_contrast_policy_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_contrast_policy_is_valid(&item)) return 2;
    if (UmiAppearanceContrastPolicyTransferCases(&item) != 0) return 1;

    return 0;
}
