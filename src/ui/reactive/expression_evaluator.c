/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/expression_evaluator.c
 *
 * PURPOSE:
 *   Implement deterministic expression evaluation outcome.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_evaluator.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the expression evaluator contract to deterministic zero/default state. */
void umi_ui_reactive_expression_evaluator_init(UmiUiReactiveExpressionEvaluator *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_expression_evaluator_valid(const UmiUiReactiveExpressionEvaluator *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->result.string_value, '\0', sizeof(item->result.string_value)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveExpressionEvaluatorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdc10cde94a5f825c);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveExpressionEvaluator *)0)->result.string_value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveExpressionEvaluatorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiUiReactiveExpressionEvaluator *)0)->result.string_value) - 1U +
        8U +
        8U;
}
static void UmiUiReactiveExpressionEvaluatorArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveExpressionEvaluator *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->result.kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->result.boolean_value);
    UmiArchiveWriteSigned(writer, (int64_t)value->result.integer_value);
    UmiArchiveWriteDouble(writer, value->result.real_value);
    UmiArchiveWriteText(writer, value->result.string_value, sizeof(value->result.string_value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->instructions_executed);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->success);
}
static void UmiUiReactiveExpressionEvaluatorArchiveRead(UmiArchiveReader *reader, UmiUiReactiveExpressionEvaluator *value)
{
    value->result.kind = (UmiUiValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->result.boolean_value = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->result.integer_value = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->result.real_value = UmiArchiveReadDouble(reader);
    UmiArchiveReadText(reader, value->result.string_value, sizeof(value->result.string_value));
    value->instructions_executed = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->success = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveExpressionEvaluatorArchiveValidate(const UmiUiReactiveExpressionEvaluator *value)
{
    return umi_ui_reactive_expression_evaluator_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_expression_evaluator_archive_encode, umi_ui_reactive_expression_evaluator_archive_decode,
    UmiUiReactiveExpressionEvaluator, UmiUiReactiveExpressionEvaluatorArchiveSchema, UmiUiReactiveExpressionEvaluatorArchiveBound, UmiUiReactiveExpressionEvaluatorArchiveWrite, UmiUiReactiveExpressionEvaluatorArchiveRead, UmiUiReactiveExpressionEvaluatorArchiveValidate)
