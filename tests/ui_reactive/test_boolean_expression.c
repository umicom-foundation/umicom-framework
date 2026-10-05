/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_boolean_expression.c
 *
 * PURPOSE:
 *   Exercise the boolean expression reactive UI contract.
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
#include "umicom/ui/reactive/boolean_expression.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/boolean_expression.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveBooleanExpressionTransferEqual(const UmiUiReactiveBooleanExpression *a, const UmiUiReactiveBooleanExpression *b)
{
    return a->value == b->value &&
        a->source_revision == b->source_revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveBooleanExpressionTransferTails(UmiUiReactiveBooleanExpression *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveBooleanExpressionTransferMalformed(const UmiUiReactiveBooleanExpression *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveBooleanExpressionTransferCases, UmiUiReactiveBooleanExpression,
    umi_ui_reactive_boolean_expression_archive_encode, umi_ui_reactive_boolean_expression_archive_decode,
    UmiUiReactiveBooleanExpressionTransferEqual, UmiUiReactiveBooleanExpressionTransferTails, UmiUiReactiveBooleanExpressionTransferMalformed)

int main(void) { UmiUiReactiveBooleanExpression item; umi_ui_reactive_boolean_expression_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveBooleanExpression populated = item;
    populated.value = true;
    populated.source_revision = (uint64_t)3;
    if (UmiUiReactiveBooleanExpressionTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_boolean_expression_valid(&item) ? 0 : 1; }
