/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_validation_rule.c
 *
 * PURPOSE:
 *   Exercise the validation rule reactive UI contract.
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
#include "umicom/ui/reactive/validation_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/validation_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveValidationRuleTransferEqual(const UmiUiReactiveValidationRule *a, const UmiUiReactiveValidationRule *b)
{
    return strcmp(a->rule_id, b->rule_id) == 0 &&
        a->required == b->required &&
        a->minimum == b->minimum &&
        a->maximum == b->maximum &&
        a->min_length == b->min_length &&
        a->max_length == b->max_length;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveValidationRuleTransferTails(UmiUiReactiveValidationRule *value)
{
    (void)value;
    {
        size_t used = strlen(value->rule_id) + 1U;
        memset(value->rule_id + used, 0xa5, sizeof(value->rule_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveValidationRuleTransferMalformed(const UmiUiReactiveValidationRule *sample)
{
    (void)sample;
    {
        UmiUiReactiveValidationRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.rule_id, 'x', sizeof(invalid.rule_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_validation_rule_valid(&invalid)) ||
            umi_ui_reactive_validation_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated rule_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveValidationRuleTransferCases, UmiUiReactiveValidationRule,
    umi_ui_reactive_validation_rule_archive_encode, umi_ui_reactive_validation_rule_archive_decode,
    UmiUiReactiveValidationRuleTransferEqual, UmiUiReactiveValidationRuleTransferTails, UmiUiReactiveValidationRuleTransferMalformed)

int main(void) { UmiUiReactiveValidationRule item; umi_ui_reactive_validation_rule_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveValidationRule populated = item;
    (void)snprintf(populated.rule_id, sizeof(populated.rule_id), "field-0");
    populated.required = true;
    populated.minimum = 4.25;
    populated.maximum = 5.25;
    populated.min_length = (size_t)6;
    populated.max_length = (size_t)7;
    if (UmiUiReactiveValidationRuleTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_validation_rule_valid(&item) ? 0 : 1; }
