/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_parameter.c
 *
 * PURPOSE:
 *   Represent a typed bound query parameter without embedding values in generated SQL.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_parameter.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_query_parameter_init(UmiDataQueryParameter *item, const char *parameter_id, const char *name, UmiDataValueKind kind, const char *value, bool sensitive) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->parameter_id,sizeof(item->parameter_id),parameter_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->name,sizeof(item->name),name);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->value,sizeof(item->value),value);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->kind=kind;item->sensitive=sensitive;
    return umi_data_query_parameter_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_query_parameter_validate(const UmiDataQueryParameter *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->parameter_id, '\0', sizeof(item->parameter_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->value, '\0', sizeof(item->value)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->parameter_id[0] != '\0' && item->name[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataQueryParameterArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2b939d85b3bbb3eb);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryParameter *)0)->parameter_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryParameter *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryParameter *)0)->value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataQueryParameterArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataQueryParameter *)0)->parameter_id) - 1U +
        8U + sizeof(((UmiDataQueryParameter *)0)->name) - 1U +
        8U +
        8U + sizeof(((UmiDataQueryParameter *)0)->value) - 1U +
        8U;
}
static void UmiDataQueryParameterArchiveWrite(UmiArchiveWriter *writer, const UmiDataQueryParameter *value)
{
    UmiArchiveWriteText(writer, value->parameter_id, sizeof(value->parameter_id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteText(writer, value->value, sizeof(value->value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sensitive);
}
static void UmiDataQueryParameterArchiveRead(UmiArchiveReader *reader, UmiDataQueryParameter *value)
{
    UmiArchiveReadText(reader, value->parameter_id, sizeof(value->parameter_id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->kind = (UmiDataValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->value, sizeof(value->value));
    value->sensitive = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataQueryParameterArchiveValidate(const UmiDataQueryParameter *value)
{
    return umi_data_query_parameter_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_query_parameter_archive_encode, umi_data_query_parameter_archive_decode,
    UmiDataQueryParameter, UmiDataQueryParameterArchiveSchema, UmiDataQueryParameterArchiveBound, UmiDataQueryParameterArchiveWrite, UmiDataQueryParameterArchiveRead, UmiDataQueryParameterArchiveValidate)
