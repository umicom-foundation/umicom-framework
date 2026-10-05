/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/entity_descriptor.c
 *
 * PURPOSE:
 *   Describe an ORM entity without coupling persistence mapping to application structs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/entity_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_entity_descriptor_init(UmiDataEntityDescriptor *item, const char *entity_id, const char *table_id, const char *identity_field) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->entity_id,sizeof(item->entity_id),entity_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->table_id,sizeof(item->table_id),table_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->identity_field,sizeof(item->identity_field),identity_field);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->field_count=0U;item->immutable=false;
    return umi_data_entity_descriptor_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_entity_descriptor_validate(const UmiDataEntityDescriptor *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->entity_id, '\0', sizeof(item->entity_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->table_id, '\0', sizeof(item->table_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->identity_field, '\0', sizeof(item->identity_field)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->entity_id[0] != '\0' && item->table_id[0] != '\0' && item->identity_field[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataEntityDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7bc07bdb592d3061);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataEntityDescriptor *)0)->entity_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataEntityDescriptor *)0)->table_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataEntityDescriptor *)0)->identity_field)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataEntityDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataEntityDescriptor *)0)->entity_id) - 1U +
        8U + sizeof(((UmiDataEntityDescriptor *)0)->table_id) - 1U +
        8U + sizeof(((UmiDataEntityDescriptor *)0)->identity_field) - 1U +
        8U +
        8U;
}
static void UmiDataEntityDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiDataEntityDescriptor *value)
{
    UmiArchiveWriteText(writer, value->entity_id, sizeof(value->entity_id));
    UmiArchiveWriteText(writer, value->table_id, sizeof(value->table_id));
    UmiArchiveWriteText(writer, value->identity_field, sizeof(value->identity_field));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->field_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->immutable);
}
static void UmiDataEntityDescriptorArchiveRead(UmiArchiveReader *reader, UmiDataEntityDescriptor *value)
{
    UmiArchiveReadText(reader, value->entity_id, sizeof(value->entity_id));
    UmiArchiveReadText(reader, value->table_id, sizeof(value->table_id));
    UmiArchiveReadText(reader, value->identity_field, sizeof(value->identity_field));
    value->field_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->immutable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataEntityDescriptorArchiveValidate(const UmiDataEntityDescriptor *value)
{
    return umi_data_entity_descriptor_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_entity_descriptor_archive_encode, umi_data_entity_descriptor_archive_decode,
    UmiDataEntityDescriptor, UmiDataEntityDescriptorArchiveSchema, UmiDataEntityDescriptorArchiveBound, UmiDataEntityDescriptorArchiveWrite, UmiDataEntityDescriptorArchiveRead, UmiDataEntityDescriptorArchiveValidate)
