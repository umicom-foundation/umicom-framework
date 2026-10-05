/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/field_mapping.c
 *
 * PURPOSE:
 *   Map one logical entity field to portable column metadata and conversion semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/field_mapping.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_field_mapping_init(UmiDataFieldMapping *item, const char *mapping_id, const char *entity_id, const char *field_name, const char *column_name, UmiDataValueKind kind) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->mapping_id,sizeof(item->mapping_id),mapping_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->entity_id,sizeof(item->entity_id),entity_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->field_name,sizeof(item->field_name),field_name);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->column_name,sizeof(item->column_name),column_name);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->kind=kind;item->nullable=false;
    return umi_data_field_mapping_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_field_mapping_validate(const UmiDataFieldMapping *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->mapping_id, '\0', sizeof(item->mapping_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->entity_id, '\0', sizeof(item->entity_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->field_name, '\0', sizeof(item->field_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->column_name, '\0', sizeof(item->column_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->mapping_id[0] != '\0' && item->entity_id[0] != '\0' && item->field_name[0] != '\0' && item->column_name[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataFieldMappingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x13d08a023fc7bc06);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataFieldMapping *)0)->mapping_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataFieldMapping *)0)->entity_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataFieldMapping *)0)->field_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataFieldMapping *)0)->column_name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataFieldMappingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataFieldMapping *)0)->mapping_id) - 1U +
        8U + sizeof(((UmiDataFieldMapping *)0)->entity_id) - 1U +
        8U + sizeof(((UmiDataFieldMapping *)0)->field_name) - 1U +
        8U + sizeof(((UmiDataFieldMapping *)0)->column_name) - 1U +
        8U +
        8U;
}
static void UmiDataFieldMappingArchiveWrite(UmiArchiveWriter *writer, const UmiDataFieldMapping *value)
{
    UmiArchiveWriteText(writer, value->mapping_id, sizeof(value->mapping_id));
    UmiArchiveWriteText(writer, value->entity_id, sizeof(value->entity_id));
    UmiArchiveWriteText(writer, value->field_name, sizeof(value->field_name));
    UmiArchiveWriteText(writer, value->column_name, sizeof(value->column_name));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->nullable);
}
static void UmiDataFieldMappingArchiveRead(UmiArchiveReader *reader, UmiDataFieldMapping *value)
{
    UmiArchiveReadText(reader, value->mapping_id, sizeof(value->mapping_id));
    UmiArchiveReadText(reader, value->entity_id, sizeof(value->entity_id));
    UmiArchiveReadText(reader, value->field_name, sizeof(value->field_name));
    UmiArchiveReadText(reader, value->column_name, sizeof(value->column_name));
    value->kind = (UmiDataValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->nullable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataFieldMappingArchiveValidate(const UmiDataFieldMapping *value)
{
    return umi_data_field_mapping_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_field_mapping_archive_encode, umi_data_field_mapping_archive_decode,
    UmiDataFieldMapping, UmiDataFieldMappingArchiveSchema, UmiDataFieldMappingArchiveBound, UmiDataFieldMappingArchiveWrite, UmiDataFieldMappingArchiveRead, UmiDataFieldMappingArchiveValidate)
