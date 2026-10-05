/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_expression.c
 *
 * PURPOSE:
 *   Represent a portable query expression node for later backend translation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_expression.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_query_expression_init(UmiDataQueryExpression *item, const char *expression_id, const char *field, const char *operation, const char *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->expression_id,sizeof(item->expression_id),expression_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->field,sizeof(item->field),field);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->operation,sizeof(item->operation),operation);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->value,sizeof(item->value),value);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->parameterized=true;
    return umi_data_query_expression_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_query_expression_validate(const UmiDataQueryExpression *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->expression_id, '\0', sizeof(item->expression_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->field, '\0', sizeof(item->field)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->operation, '\0', sizeof(item->operation)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->value, '\0', sizeof(item->value)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->expression_id[0] != '\0' && item->field[0] != '\0' && item->operation[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataQueryExpressionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1fbc0b3a333108a8);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryExpression *)0)->expression_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryExpression *)0)->field)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryExpression *)0)->operation)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryExpression *)0)->value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataQueryExpressionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataQueryExpression *)0)->expression_id) - 1U +
        8U + sizeof(((UmiDataQueryExpression *)0)->field) - 1U +
        8U + sizeof(((UmiDataQueryExpression *)0)->operation) - 1U +
        8U + sizeof(((UmiDataQueryExpression *)0)->value) - 1U +
        8U;
}
static void UmiDataQueryExpressionArchiveWrite(UmiArchiveWriter *writer, const UmiDataQueryExpression *value)
{
    UmiArchiveWriteText(writer, value->expression_id, sizeof(value->expression_id));
    UmiArchiveWriteText(writer, value->field, sizeof(value->field));
    UmiArchiveWriteText(writer, value->operation, sizeof(value->operation));
    UmiArchiveWriteText(writer, value->value, sizeof(value->value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->parameterized);
}
static void UmiDataQueryExpressionArchiveRead(UmiArchiveReader *reader, UmiDataQueryExpression *value)
{
    UmiArchiveReadText(reader, value->expression_id, sizeof(value->expression_id));
    UmiArchiveReadText(reader, value->field, sizeof(value->field));
    UmiArchiveReadText(reader, value->operation, sizeof(value->operation));
    UmiArchiveReadText(reader, value->value, sizeof(value->value));
    value->parameterized = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataQueryExpressionArchiveValidate(const UmiDataQueryExpression *value)
{
    return umi_data_query_expression_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_query_expression_archive_encode, umi_data_query_expression_archive_decode,
    UmiDataQueryExpression, UmiDataQueryExpressionArchiveSchema, UmiDataQueryExpressionArchiveBound, UmiDataQueryExpressionArchiveWrite, UmiDataQueryExpressionArchiveRead, UmiDataQueryExpressionArchiveValidate)
