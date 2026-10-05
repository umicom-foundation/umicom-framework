/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/result_mapping.c
 *
 * PURPOSE:
 *   Describe how one result column maps back into an ORM field.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/result_mapping.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_result_mapping_init(UmiDataResultMapping *item, const char *mapping_id, const char *entity_id, const char *field_name, uint32_t column_ordinal, UmiDataValueKind kind) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->mapping_id,sizeof(item->mapping_id),mapping_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->entity_id,sizeof(item->entity_id),entity_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->field_name,sizeof(item->field_name),field_name);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->column_ordinal=column_ordinal;item->kind=kind;
    return umi_data_result_mapping_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_result_mapping_validate(const UmiDataResultMapping *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->mapping_id, '\0', sizeof(item->mapping_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->entity_id, '\0', sizeof(item->entity_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->field_name, '\0', sizeof(item->field_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->mapping_id[0] != '\0' && item->entity_id[0] != '\0' && item->field_name[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataResultMappingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6bded8a569b5ad0d);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataResultMapping *)0)->mapping_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataResultMapping *)0)->entity_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataResultMapping *)0)->field_name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataResultMappingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataResultMapping *)0)->mapping_id) - 1U +
        8U + sizeof(((UmiDataResultMapping *)0)->entity_id) - 1U +
        8U + sizeof(((UmiDataResultMapping *)0)->field_name) - 1U +
        8U +
        8U;
}
static void UmiDataResultMappingArchiveWrite(UmiArchiveWriter *writer, const UmiDataResultMapping *value)
{
    UmiArchiveWriteText(writer, value->mapping_id, sizeof(value->mapping_id));
    UmiArchiveWriteText(writer, value->entity_id, sizeof(value->entity_id));
    UmiArchiveWriteText(writer, value->field_name, sizeof(value->field_name));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->column_ordinal);
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
}
static void UmiDataResultMappingArchiveRead(UmiArchiveReader *reader, UmiDataResultMapping *value)
{
    UmiArchiveReadText(reader, value->mapping_id, sizeof(value->mapping_id));
    UmiArchiveReadText(reader, value->entity_id, sizeof(value->entity_id));
    UmiArchiveReadText(reader, value->field_name, sizeof(value->field_name));
    value->column_ordinal = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->kind = (UmiDataValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDataResultMappingArchiveValidate(const UmiDataResultMapping *value)
{
    return umi_data_result_mapping_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_result_mapping_archive_encode, umi_data_result_mapping_archive_decode,
    UmiDataResultMapping, UmiDataResultMappingArchiveSchema, UmiDataResultMappingArchiveBound, UmiDataResultMappingArchiveWrite, UmiDataResultMappingArchiveRead, UmiDataResultMappingArchiveValidate)
