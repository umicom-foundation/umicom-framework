/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_order.c
 *
 * PURPOSE:
 *   Describe deterministic query ordering independent of SQL dialect.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_order.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_query_order_init(UmiDataQueryOrder *item, const char *order_id, const char *field, bool descending) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->order_id,sizeof(item->order_id),order_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->field,sizeof(item->field),field);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->descending=descending;item->nulls_last=true;
    return umi_data_query_order_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_query_order_validate(const UmiDataQueryOrder *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->order_id, '\0', sizeof(item->order_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->field, '\0', sizeof(item->field)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->order_id[0] != '\0' && item->field[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataQueryOrderArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x80d0f9805aa3346f);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryOrder *)0)->order_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryOrder *)0)->field)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataQueryOrderArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataQueryOrder *)0)->order_id) - 1U +
        8U + sizeof(((UmiDataQueryOrder *)0)->field) - 1U +
        8U +
        8U;
}
static void UmiDataQueryOrderArchiveWrite(UmiArchiveWriter *writer, const UmiDataQueryOrder *value)
{
    UmiArchiveWriteText(writer, value->order_id, sizeof(value->order_id));
    UmiArchiveWriteText(writer, value->field, sizeof(value->field));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->descending);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->nulls_last);
}
static void UmiDataQueryOrderArchiveRead(UmiArchiveReader *reader, UmiDataQueryOrder *value)
{
    UmiArchiveReadText(reader, value->order_id, sizeof(value->order_id));
    UmiArchiveReadText(reader, value->field, sizeof(value->field));
    value->descending = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->nulls_last = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataQueryOrderArchiveValidate(const UmiDataQueryOrder *value)
{
    return umi_data_query_order_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_query_order_archive_encode, umi_data_query_order_archive_decode,
    UmiDataQueryOrder, UmiDataQueryOrderArchiveSchema, UmiDataQueryOrderArchiveBound, UmiDataQueryOrderArchiveWrite, UmiDataQueryOrderArchiveRead, UmiDataQueryOrderArchiveValidate)
