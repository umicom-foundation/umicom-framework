/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/expression_operand.c
 *
 * PURPOSE:
 *   Implement scalar expression operands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_operand.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the expression operand contract to deterministic zero/default state. */
void umi_ui_reactive_expression_operand_init(UmiUiReactiveExpressionOperand *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_expression_operand_valid(const UmiUiReactiveExpressionOperand *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->value.string_value, '\0', sizeof(item->value.string_value)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveExpressionOperandArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xeade9fd3b35aeeeb);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveExpressionOperand *)0)->value.string_value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveExpressionOperandArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiUiReactiveExpressionOperand *)0)->value.string_value) - 1U +
        8U;
}
static void UmiUiReactiveExpressionOperandArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveExpressionOperand *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->value.kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->value.boolean_value);
    UmiArchiveWriteSigned(writer, (int64_t)value->value.integer_value);
    UmiArchiveWriteDouble(writer, value->value.real_value);
    UmiArchiveWriteText(writer, value->value.string_value, sizeof(value->value.string_value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->resolved);
}
static void UmiUiReactiveExpressionOperandArchiveRead(UmiArchiveReader *reader, UmiUiReactiveExpressionOperand *value)
{
    value->value.kind = (UmiUiValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->value.boolean_value = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->value.integer_value = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->value.real_value = UmiArchiveReadDouble(reader);
    UmiArchiveReadText(reader, value->value.string_value, sizeof(value->value.string_value));
    value->resolved = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveExpressionOperandArchiveValidate(const UmiUiReactiveExpressionOperand *value)
{
    return umi_ui_reactive_expression_operand_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_expression_operand_archive_encode, umi_ui_reactive_expression_operand_archive_decode,
    UmiUiReactiveExpressionOperand, UmiUiReactiveExpressionOperandArchiveSchema, UmiUiReactiveExpressionOperandArchiveBound, UmiUiReactiveExpressionOperandArchiveWrite, UmiUiReactiveExpressionOperandArchiveRead, UmiUiReactiveExpressionOperandArchiveValidate)
