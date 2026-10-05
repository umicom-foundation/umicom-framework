/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/expression_operator.c
 *
 * PURPOSE:
 *   Implement supported comparison and logical operators.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_operator.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the expression operator contract to deterministic zero/default state. */
void umi_ui_reactive_expression_operator_init(UmiUiReactiveExpressionOperator *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_expression_operator_valid(const UmiUiReactiveExpressionOperator *item) {
    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveExpressionOperatorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2e69c0c39385ce3f);

    return schema;
}
static size_t UmiUiReactiveExpressionOperatorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U;
}
static void UmiUiReactiveExpressionOperatorArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveExpressionOperator *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->op);
    UmiArchiveWriteSigned(writer, (int64_t)value->precedence);
}
static void UmiUiReactiveExpressionOperatorArchiveRead(UmiArchiveReader *reader, UmiUiReactiveExpressionOperator *value)
{
    value->op = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->precedence = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiUiReactiveExpressionOperatorArchiveValidate(const UmiUiReactiveExpressionOperator *value)
{
    return umi_ui_reactive_expression_operator_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_expression_operator_archive_encode, umi_ui_reactive_expression_operator_archive_decode,
    UmiUiReactiveExpressionOperator, UmiUiReactiveExpressionOperatorArchiveSchema, UmiUiReactiveExpressionOperatorArchiveBound, UmiUiReactiveExpressionOperatorArchiveWrite, UmiUiReactiveExpressionOperatorArchiveRead, UmiUiReactiveExpressionOperatorArchiveValidate)
