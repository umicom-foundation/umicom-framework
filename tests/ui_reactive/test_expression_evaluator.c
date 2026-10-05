/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_evaluator.c
 *
 * PURPOSE:
 *   Exercise the expression evaluator reactive UI contract.
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
#include "umicom/ui/reactive/expression_evaluator.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/expression_evaluator.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveExpressionEvaluatorTransferEqual(const UmiUiReactiveExpressionEvaluator *a, const UmiUiReactiveExpressionEvaluator *b)
{
    return a->result.kind == b->result.kind &&
        a->result.boolean_value == b->result.boolean_value &&
        a->result.integer_value == b->result.integer_value &&
        a->result.real_value == b->result.real_value &&
        strcmp(a->result.string_value, b->result.string_value) == 0 &&
        a->instructions_executed == b->instructions_executed &&
        a->success == b->success;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveExpressionEvaluatorTransferTails(UmiUiReactiveExpressionEvaluator *value)
{
    (void)value;
    {
        size_t used = strlen(value->result.string_value) + 1U;
        memset(value->result.string_value + used, 0xa5, sizeof(value->result.string_value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveExpressionEvaluatorTransferMalformed(const UmiUiReactiveExpressionEvaluator *sample)
{
    (void)sample;
    {
        UmiUiReactiveExpressionEvaluator invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.result.string_value, 'x', sizeof(invalid.result.string_value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_expression_evaluator_valid(&invalid)) ||
            umi_ui_reactive_expression_evaluator_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated result.string_value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveExpressionEvaluatorTransferCases, UmiUiReactiveExpressionEvaluator,
    umi_ui_reactive_expression_evaluator_archive_encode, umi_ui_reactive_expression_evaluator_archive_decode,
    UmiUiReactiveExpressionEvaluatorTransferEqual, UmiUiReactiveExpressionEvaluatorTransferTails, UmiUiReactiveExpressionEvaluatorTransferMalformed)

int main(void) { UmiUiReactiveExpressionEvaluator item; umi_ui_reactive_expression_evaluator_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveExpressionEvaluator populated = item;
    populated.result.boolean_value = (int)3;
    populated.result.integer_value = (int64_t)4;
    populated.result.real_value = 5.25;
    (void)snprintf(populated.result.string_value, sizeof(populated.result.string_value), "field-4");
    populated.instructions_executed = (size_t)7;
    populated.success = true;
    if (UmiUiReactiveExpressionEvaluatorTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_expression_evaluator_valid(&item) ? 0 : 1; }
