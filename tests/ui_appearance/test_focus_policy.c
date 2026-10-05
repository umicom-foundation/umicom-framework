/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_focus_policy.c
 *
 * PURPOSE:
 *   Verify define visible keyboard-focus treatment requirements across all renderer adapters.
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
#include "umicom/ui/appearance/focus_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/focus_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceFocusPolicyTransferEqual(const UmiAppearanceFocusPolicy *a, const UmiAppearanceFocusPolicy *b)
{
    return strcmp(a->policy_id, b->policy_id) == 0 &&
        a->ring_width == b->ring_width &&
        a->ring_offset == b->ring_offset &&
        a->always_visible_for_keyboard == b->always_visible_for_keyboard &&
        a->clip_safe == b->clip_safe;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceFocusPolicyTransferTails(UmiAppearanceFocusPolicy *value)
{
    (void)value;
    {
        size_t used = strlen(value->policy_id) + 1U;
        memset(value->policy_id + used, 0xa5, sizeof(value->policy_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceFocusPolicyTransferMalformed(const UmiAppearanceFocusPolicy *sample)
{
    (void)sample;
    {
        UmiAppearanceFocusPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.policy_id, 'x', sizeof(invalid.policy_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_focus_policy_is_valid(&invalid)) ||
            umi_appearance_focus_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated policy_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceFocusPolicyTransferCases, UmiAppearanceFocusPolicy,
    umi_appearance_focus_policy_archive_encode, umi_appearance_focus_policy_archive_decode,
    UmiAppearanceFocusPolicyTransferEqual, UmiAppearanceFocusPolicyTransferTails, UmiAppearanceFocusPolicyTransferMalformed)

int main(void) {
    UmiAppearanceFocusPolicy item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_focus_policy_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_focus_policy_is_valid(&item)) return 2;
    if (UmiAppearanceFocusPolicyTransferCases(&item) != 0) return 1;

    return 0;
}
