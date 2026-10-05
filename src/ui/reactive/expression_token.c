/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/expression_token.c
 *
 * PURPOSE:
 *   Implement a compact token used by renderer-neutral state expressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_token.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the expression token contract to deterministic zero/default state. */
void umi_ui_reactive_expression_token_init(UmiUiReactiveExpressionToken *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_expression_token_valid(const UmiUiReactiveExpressionToken *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->text, '\0', sizeof(item->text)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveExpressionTokenArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x29896f67024dac3b);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveExpressionToken *)0)->text)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveExpressionTokenArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiUiReactiveExpressionToken *)0)->text) - 1U +
        8U;
}
static void UmiUiReactiveExpressionTokenArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveExpressionToken *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteText(writer, value->text, sizeof(value->text));
    UmiArchiveWriteDouble(writer, value->number);
}
static void UmiUiReactiveExpressionTokenArchiveRead(UmiArchiveReader *reader, UmiUiReactiveExpressionToken *value)
{
    value->kind = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->text, sizeof(value->text));
    value->number = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiUiReactiveExpressionTokenArchiveValidate(const UmiUiReactiveExpressionToken *value)
{
    return umi_ui_reactive_expression_token_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_expression_token_archive_encode, umi_ui_reactive_expression_token_archive_decode,
    UmiUiReactiveExpressionToken, UmiUiReactiveExpressionTokenArchiveSchema, UmiUiReactiveExpressionTokenArchiveBound, UmiUiReactiveExpressionTokenArchiveWrite, UmiUiReactiveExpressionTokenArchiveRead, UmiUiReactiveExpressionTokenArchiveValidate)
