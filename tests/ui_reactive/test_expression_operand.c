/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_operand.c
 *
 * PURPOSE:
 *   Exercise the expression operand reactive UI contract.
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
#include "umicom/ui/reactive/expression_operand.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/expression_operand.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveExpressionOperandTransferEqual(const UmiUiReactiveExpressionOperand *a, const UmiUiReactiveExpressionOperand *b)
{
    return a->value.kind == b->value.kind &&
        a->value.boolean_value == b->value.boolean_value &&
        a->value.integer_value == b->value.integer_value &&
        a->value.real_value == b->value.real_value &&
        strcmp(a->value.string_value, b->value.string_value) == 0 &&
        a->resolved == b->resolved;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveExpressionOperandTransferTails(UmiUiReactiveExpressionOperand *value)
{
    (void)value;
    {
        size_t used = strlen(value->value.string_value) + 1U;
        memset(value->value.string_value + used, 0xa5, sizeof(value->value.string_value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveExpressionOperandTransferMalformed(const UmiUiReactiveExpressionOperand *sample)
{
    (void)sample;
    {
        UmiUiReactiveExpressionOperand invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.string_value, 'x', sizeof(invalid.value.string_value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_expression_operand_valid(&invalid)) ||
            umi_ui_reactive_expression_operand_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.string_value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveExpressionOperandTransferCases, UmiUiReactiveExpressionOperand,
    umi_ui_reactive_expression_operand_archive_encode, umi_ui_reactive_expression_operand_archive_decode,
    UmiUiReactiveExpressionOperandTransferEqual, UmiUiReactiveExpressionOperandTransferTails, UmiUiReactiveExpressionOperandTransferMalformed)

int main(void) { UmiUiReactiveExpressionOperand item; umi_ui_reactive_expression_operand_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveExpressionOperand populated = item;
    populated.value.boolean_value = (int)3;
    populated.value.integer_value = (int64_t)4;
    populated.value.real_value = 5.25;
    (void)snprintf(populated.value.string_value, sizeof(populated.value.string_value), "field-4");
    populated.resolved = true;
    if (UmiUiReactiveExpressionOperandTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_expression_operand_valid(&item) ? 0 : 1; }
