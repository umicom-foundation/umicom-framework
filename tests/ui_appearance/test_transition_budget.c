/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_transition_budget.c
 *
 * PURPOSE:
 *   Verify bound concurrent transitions and cumulative duration to avoid animation-heavy workstation surfaces.
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
#include "umicom/ui/appearance/transition_budget.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/transition_budget.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceTransitionBudgetTransferEqual(const UmiAppearanceTransitionBudget *a, const UmiAppearanceTransitionBudget *b)
{
    return strcmp(a->budget_id, b->budget_id) == 0 &&
        a->max_concurrent == b->max_concurrent &&
        a->max_duration_ms == b->max_duration_ms &&
        a->max_delayed_ms == b->max_delayed_ms;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceTransitionBudgetTransferTails(UmiAppearanceTransitionBudget *value)
{
    (void)value;
    {
        size_t used = strlen(value->budget_id) + 1U;
        memset(value->budget_id + used, 0xa5, sizeof(value->budget_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceTransitionBudgetTransferMalformed(const UmiAppearanceTransitionBudget *sample)
{
    (void)sample;
    {
        UmiAppearanceTransitionBudget invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.budget_id, 'x', sizeof(invalid.budget_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_transition_budget_is_valid(&invalid)) ||
            umi_appearance_transition_budget_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated budget_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceTransitionBudgetTransferCases, UmiAppearanceTransitionBudget,
    umi_appearance_transition_budget_archive_encode, umi_appearance_transition_budget_archive_decode,
    UmiAppearanceTransitionBudgetTransferEqual, UmiAppearanceTransitionBudgetTransferTails, UmiAppearanceTransitionBudgetTransferMalformed)

int main(void) {
    UmiAppearanceTransitionBudget item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_transition_budget_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_transition_budget_is_valid(&item)) return 2;
    if (UmiAppearanceTransitionBudgetTransferCases(&item) != 0) return 1;

    return 0;
}
