/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_visibility_rule.c
 *
 * PURPOSE:
 *   Exercise the visibility rule reactive UI contract.
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
#include "umicom/ui/reactive/visibility_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/visibility_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveVisibilityRuleTransferEqual(const UmiUiReactiveVisibilityRule *a, const UmiUiReactiveVisibilityRule *b)
{
    return strcmp(a->target_id, b->target_id) == 0 &&
        strcmp(a->expression, b->expression) == 0 &&
        a->visible == b->visible;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveVisibilityRuleTransferTails(UmiUiReactiveVisibilityRule *value)
{
    (void)value;
    {
        size_t used = strlen(value->target_id) + 1U;
        memset(value->target_id + used, 0xa5, sizeof(value->target_id) - used);
    }
    {
        size_t used = strlen(value->expression) + 1U;
        memset(value->expression + used, 0xa5, sizeof(value->expression) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveVisibilityRuleTransferMalformed(const UmiUiReactiveVisibilityRule *sample)
{
    (void)sample;
    {
        UmiUiReactiveVisibilityRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_id, 'x', sizeof(invalid.target_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_visibility_rule_valid(&invalid)) ||
            umi_ui_reactive_visibility_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveVisibilityRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.expression, 'x', sizeof(invalid.expression));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_visibility_rule_valid(&invalid)) ||
            umi_ui_reactive_visibility_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated expression was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveVisibilityRuleTransferCases, UmiUiReactiveVisibilityRule,
    umi_ui_reactive_visibility_rule_archive_encode, umi_ui_reactive_visibility_rule_archive_decode,
    UmiUiReactiveVisibilityRuleTransferEqual, UmiUiReactiveVisibilityRuleTransferTails, UmiUiReactiveVisibilityRuleTransferMalformed)

int main(void) { UmiUiReactiveVisibilityRule item; umi_ui_reactive_visibility_rule_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveVisibilityRule populated = item;
    (void)snprintf(populated.target_id, sizeof(populated.target_id), "field-0");
    (void)snprintf(populated.expression, sizeof(populated.expression), "field-1");
    populated.visible = true;
    if (UmiUiReactiveVisibilityRuleTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_visibility_rule_valid(&item) ? 0 : 1; }
