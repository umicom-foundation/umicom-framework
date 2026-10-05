/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/expression_lexer.c
 *
 * PURPOSE:
 *   Track bounded lexical state for declarative expressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_lexer.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the expression lexer contract to deterministic zero/default state. */
void umi_ui_reactive_expression_lexer_init(UmiUiReactiveExpressionLexer *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_expression_lexer_valid(const UmiUiReactiveExpressionLexer *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->source, '\0', sizeof(item->source)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveExpressionLexerArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x259e7a49563456a1);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveExpressionLexer *)0)->source)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveExpressionLexerArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveExpressionLexer *)0)->source) - 1U +
        8U +
        8U;
}
static void UmiUiReactiveExpressionLexerArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveExpressionLexer *value)
{
    UmiArchiveWriteText(writer, value->source, sizeof(value->source));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->offset);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->token_count);
}
static void UmiUiReactiveExpressionLexerArchiveRead(UmiArchiveReader *reader, UmiUiReactiveExpressionLexer *value)
{
    UmiArchiveReadText(reader, value->source, sizeof(value->source));
    value->offset = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->token_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
}
static UmiStatus UmiUiReactiveExpressionLexerArchiveValidate(const UmiUiReactiveExpressionLexer *value)
{
    return umi_ui_reactive_expression_lexer_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_expression_lexer_archive_encode, umi_ui_reactive_expression_lexer_archive_decode,
    UmiUiReactiveExpressionLexer, UmiUiReactiveExpressionLexerArchiveSchema, UmiUiReactiveExpressionLexerArchiveBound, UmiUiReactiveExpressionLexerArchiveWrite, UmiUiReactiveExpressionLexerArchiveRead, UmiUiReactiveExpressionLexerArchiveValidate)
