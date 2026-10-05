/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_token.c
 *
 * PURPOSE:
 *   Exercise the expression token reactive UI contract.
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
#include "umicom/ui/reactive/expression_token.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/expression_token.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveExpressionTokenTransferEqual(const UmiUiReactiveExpressionToken *a, const UmiUiReactiveExpressionToken *b)
{
    return a->kind == b->kind &&
        strcmp(a->text, b->text) == 0 &&
        a->number == b->number;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveExpressionTokenTransferTails(UmiUiReactiveExpressionToken *value)
{
    (void)value;
    {
        size_t used = strlen(value->text) + 1U;
        memset(value->text + used, 0xa5, sizeof(value->text) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveExpressionTokenTransferMalformed(const UmiUiReactiveExpressionToken *sample)
{
    (void)sample;
    {
        UmiUiReactiveExpressionToken invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.text, 'x', sizeof(invalid.text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_expression_token_valid(&invalid)) ||
            umi_ui_reactive_expression_token_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated text was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveExpressionTokenTransferCases, UmiUiReactiveExpressionToken,
    umi_ui_reactive_expression_token_archive_encode, umi_ui_reactive_expression_token_archive_decode,
    UmiUiReactiveExpressionTokenTransferEqual, UmiUiReactiveExpressionTokenTransferTails, UmiUiReactiveExpressionTokenTransferMalformed)

int main(void) { UmiUiReactiveExpressionToken item; umi_ui_reactive_expression_token_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveExpressionToken populated = item;
    populated.kind = (int)2;
    (void)snprintf(populated.text, sizeof(populated.text), "field-1");
    populated.number = 4.25;
    if (UmiUiReactiveExpressionTokenTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_expression_token_valid(&item) ? 0 : 1; }
