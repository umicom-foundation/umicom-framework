/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/relation_mapping.c
 *
 * PURPOSE:
 *   Describe entity relations independently from SQL foreign-key syntax.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/relation_mapping.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_relation_mapping_init(UmiDataRelationMapping *item, const char *relation_id, const char *source_entity, const char *target_entity, const char *source_field, bool collection) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->relation_id,sizeof(item->relation_id),relation_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->source_entity,sizeof(item->source_entity),source_entity);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->target_entity,sizeof(item->target_entity),target_entity);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->source_field,sizeof(item->source_field),source_field);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->collection=collection;item->required=false;
    return umi_data_relation_mapping_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_relation_mapping_validate(const UmiDataRelationMapping *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->relation_id, '\0', sizeof(item->relation_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->source_entity, '\0', sizeof(item->source_entity)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->target_entity, '\0', sizeof(item->target_entity)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->source_field, '\0', sizeof(item->source_field)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->relation_id[0] != '\0' && item->source_entity[0] != '\0' && item->target_entity[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataRelationMappingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1a6e7ea2285c7291);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataRelationMapping *)0)->relation_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataRelationMapping *)0)->source_entity)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataRelationMapping *)0)->target_entity)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataRelationMapping *)0)->source_field)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataRelationMappingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataRelationMapping *)0)->relation_id) - 1U +
        8U + sizeof(((UmiDataRelationMapping *)0)->source_entity) - 1U +
        8U + sizeof(((UmiDataRelationMapping *)0)->target_entity) - 1U +
        8U + sizeof(((UmiDataRelationMapping *)0)->source_field) - 1U +
        8U +
        8U;
}
static void UmiDataRelationMappingArchiveWrite(UmiArchiveWriter *writer, const UmiDataRelationMapping *value)
{
    UmiArchiveWriteText(writer, value->relation_id, sizeof(value->relation_id));
    UmiArchiveWriteText(writer, value->source_entity, sizeof(value->source_entity));
    UmiArchiveWriteText(writer, value->target_entity, sizeof(value->target_entity));
    UmiArchiveWriteText(writer, value->source_field, sizeof(value->source_field));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->collection);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required);
}
static void UmiDataRelationMappingArchiveRead(UmiArchiveReader *reader, UmiDataRelationMapping *value)
{
    UmiArchiveReadText(reader, value->relation_id, sizeof(value->relation_id));
    UmiArchiveReadText(reader, value->source_entity, sizeof(value->source_entity));
    UmiArchiveReadText(reader, value->target_entity, sizeof(value->target_entity));
    UmiArchiveReadText(reader, value->source_field, sizeof(value->source_field));
    value->collection = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->required = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataRelationMappingArchiveValidate(const UmiDataRelationMapping *value)
{
    return umi_data_relation_mapping_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_relation_mapping_archive_encode, umi_data_relation_mapping_archive_decode,
    UmiDataRelationMapping, UmiDataRelationMappingArchiveSchema, UmiDataRelationMappingArchiveBound, UmiDataRelationMappingArchiveWrite, UmiDataRelationMappingArchiveRead, UmiDataRelationMappingArchiveValidate)
