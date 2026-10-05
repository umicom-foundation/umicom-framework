/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_operator.c
 *
 * PURPOSE:
 *   Exercise the expression operator reactive UI contract.
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
#include "umicom/ui/reactive/expression_operator.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/expression_operator.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveExpressionOperatorTransferEqual(const UmiUiReactiveExpressionOperator *a, const UmiUiReactiveExpressionOperator *b)
{
    return a->op == b->op &&
        a->precedence == b->precedence;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveExpressionOperatorTransferTails(UmiUiReactiveExpressionOperator *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveExpressionOperatorTransferMalformed(const UmiUiReactiveExpressionOperator *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveExpressionOperatorTransferCases, UmiUiReactiveExpressionOperator,
    umi_ui_reactive_expression_operator_archive_encode, umi_ui_reactive_expression_operator_archive_decode,
    UmiUiReactiveExpressionOperatorTransferEqual, UmiUiReactiveExpressionOperatorTransferTails, UmiUiReactiveExpressionOperatorTransferMalformed)

int main(void) { UmiUiReactiveExpressionOperator item; umi_ui_reactive_expression_operator_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveExpressionOperator populated = item;
    populated.op = (int)2;
    populated.precedence = (int)3;
    if (UmiUiReactiveExpressionOperatorTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_expression_operator_valid(&item) ? 0 : 1; }
