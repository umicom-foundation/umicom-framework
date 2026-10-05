/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_lexer.c
 *
 * PURPOSE:
 *   Exercise the expression lexer reactive UI contract.
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
#include "umicom/ui/reactive/expression_lexer.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/expression_lexer.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveExpressionLexerTransferEqual(const UmiUiReactiveExpressionLexer *a, const UmiUiReactiveExpressionLexer *b)
{
    return strcmp(a->source, b->source) == 0 &&
        a->offset == b->offset &&
        a->token_count == b->token_count;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveExpressionLexerTransferTails(UmiUiReactiveExpressionLexer *value)
{
    (void)value;
    {
        size_t used = strlen(value->source) + 1U;
        memset(value->source + used, 0xa5, sizeof(value->source) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveExpressionLexerTransferMalformed(const UmiUiReactiveExpressionLexer *sample)
{
    (void)sample;
    {
        UmiUiReactiveExpressionLexer invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source, 'x', sizeof(invalid.source));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_expression_lexer_valid(&invalid)) ||
            umi_ui_reactive_expression_lexer_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveExpressionLexerTransferCases, UmiUiReactiveExpressionLexer,
    umi_ui_reactive_expression_lexer_archive_encode, umi_ui_reactive_expression_lexer_archive_decode,
    UmiUiReactiveExpressionLexerTransferEqual, UmiUiReactiveExpressionLexerTransferTails, UmiUiReactiveExpressionLexerTransferMalformed)

int main(void) { UmiUiReactiveExpressionLexer item; umi_ui_reactive_expression_lexer_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveExpressionLexer populated = item;
    (void)snprintf(populated.source, sizeof(populated.source), "field-0");
    populated.offset = (size_t)3;
    populated.token_count = (size_t)4;
    if (UmiUiReactiveExpressionLexerTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_expression_lexer_valid(&item) ? 0 : 1; }
