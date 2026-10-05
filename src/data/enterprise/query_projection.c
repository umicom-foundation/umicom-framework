/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_projection.c
 *
 * PURPOSE:
 *   Describe one selected field/alias for portable query result shapes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_projection.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_query_projection_init(UmiDataQueryProjection *item, const char *projection_id, const char *field, const char *alias) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->projection_id,sizeof(item->projection_id),projection_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->field,sizeof(item->field),field);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->alias,sizeof(item->alias),alias);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->hidden=false;
    return umi_data_query_projection_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_query_projection_validate(const UmiDataQueryProjection *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->projection_id, '\0', sizeof(item->projection_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->field, '\0', sizeof(item->field)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->alias, '\0', sizeof(item->alias)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->projection_id[0] != '\0' && item->field[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataQueryProjectionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb5509fe91e89286b);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryProjection *)0)->projection_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryProjection *)0)->field)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryProjection *)0)->alias)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataQueryProjectionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataQueryProjection *)0)->projection_id) - 1U +
        8U + sizeof(((UmiDataQueryProjection *)0)->field) - 1U +
        8U + sizeof(((UmiDataQueryProjection *)0)->alias) - 1U +
        8U;
}
static void UmiDataQueryProjectionArchiveWrite(UmiArchiveWriter *writer, const UmiDataQueryProjection *value)
{
    UmiArchiveWriteText(writer, value->projection_id, sizeof(value->projection_id));
    UmiArchiveWriteText(writer, value->field, sizeof(value->field));
    UmiArchiveWriteText(writer, value->alias, sizeof(value->alias));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->hidden);
}
static void UmiDataQueryProjectionArchiveRead(UmiArchiveReader *reader, UmiDataQueryProjection *value)
{
    UmiArchiveReadText(reader, value->projection_id, sizeof(value->projection_id));
    UmiArchiveReadText(reader, value->field, sizeof(value->field));
    UmiArchiveReadText(reader, value->alias, sizeof(value->alias));
    value->hidden = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataQueryProjectionArchiveValidate(const UmiDataQueryProjection *value)
{
    return umi_data_query_projection_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_query_projection_archive_encode, umi_data_query_projection_archive_decode,
    UmiDataQueryProjection, UmiDataQueryProjectionArchiveSchema, UmiDataQueryProjectionArchiveBound, UmiDataQueryProjectionArchiveWrite, UmiDataQueryProjectionArchiveRead, UmiDataQueryProjectionArchiveValidate)
