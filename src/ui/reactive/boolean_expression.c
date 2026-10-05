/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/boolean_expression.c
 *
 * PURPOSE:
 *   Implement a boolean expression result with source revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/boolean_expression.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the boolean expression contract to deterministic zero/default state. */
void umi_ui_reactive_boolean_expression_init(UmiUiReactiveBooleanExpression *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_boolean_expression_valid(const UmiUiReactiveBooleanExpression *item) {
    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveBooleanExpressionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6a700b7a7347286c);

    return schema;
}
static size_t UmiUiReactiveBooleanExpressionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U;
}
static void UmiUiReactiveBooleanExpressionArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveBooleanExpression *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->source_revision);
}
static void UmiUiReactiveBooleanExpressionArchiveRead(UmiArchiveReader *reader, UmiUiReactiveBooleanExpression *value)
{
    value->value = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->source_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiUiReactiveBooleanExpressionArchiveValidate(const UmiUiReactiveBooleanExpression *value)
{
    return umi_ui_reactive_boolean_expression_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_boolean_expression_archive_encode, umi_ui_reactive_boolean_expression_archive_decode,
    UmiUiReactiveBooleanExpression, UmiUiReactiveBooleanExpressionArchiveSchema, UmiUiReactiveBooleanExpressionArchiveBound, UmiUiReactiveBooleanExpressionArchiveWrite, UmiUiReactiveBooleanExpressionArchiveRead, UmiUiReactiveBooleanExpressionArchiveValidate)
