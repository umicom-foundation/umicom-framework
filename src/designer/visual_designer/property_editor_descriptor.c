/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/property_editor_descriptor.c
 *
 * PURPOSE:
 *   Describe an editor choice for a semantic component property.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/property_editor_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer property editor descriptor from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_rad_property_editor_descriptor_init(UmiRadPropertyEditorDescriptor *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->property_id, sizeof item->property_id, "property_editor_descriptor");
    (void)umi_rad_copy_text(item->editor_type, sizeof item->editor_type, "property_editor_descriptor");
    (void)umi_rad_copy_text(item->value_type, sizeof item->value_type, "property_editor_descriptor");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer property editor descriptor satisfies its contract before another service
 * relies on it.
 */
int umi_rad_property_editor_descriptor_is_valid(const UmiRadPropertyEditorDescriptor *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->property_id, '\0', sizeof(item->property_id)) == NULL) return 0;
    if (memchr(item->editor_type, '\0', sizeof(item->editor_type)) == NULL) return 0;
    if (memchr(item->value_type, '\0', sizeof(item->value_type)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->property_id) && umi_rad_id_valid(item->editor_type);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadPropertyEditorDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf22b75220d736d99);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPropertyEditorDescriptor *)0)->property_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPropertyEditorDescriptor *)0)->editor_type)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPropertyEditorDescriptor *)0)->value_type)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadPropertyEditorDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadPropertyEditorDescriptor *)0)->property_id) - 1U +
        8U + sizeof(((UmiRadPropertyEditorDescriptor *)0)->editor_type) - 1U +
        8U + sizeof(((UmiRadPropertyEditorDescriptor *)0)->value_type) - 1U +
        8U +
        8U;
}
static void UmiRadPropertyEditorDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiRadPropertyEditorDescriptor *value)
{
    UmiArchiveWriteText(writer, value->property_id, sizeof(value->property_id));
    UmiArchiveWriteText(writer, value->editor_type, sizeof(value->editor_type));
    UmiArchiveWriteText(writer, value->value_type, sizeof(value->value_type));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->read_only);
}
static void UmiRadPropertyEditorDescriptorArchiveRead(UmiArchiveReader *reader, UmiRadPropertyEditorDescriptor *value)
{
    UmiArchiveReadText(reader, value->property_id, sizeof(value->property_id));
    UmiArchiveReadText(reader, value->editor_type, sizeof(value->editor_type));
    UmiArchiveReadText(reader, value->value_type, sizeof(value->value_type));
    value->required = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->read_only = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadPropertyEditorDescriptorArchiveValidate(const UmiRadPropertyEditorDescriptor *value)
{
    return umi_rad_property_editor_descriptor_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_property_editor_descriptor_archive_encode, umi_rad_property_editor_descriptor_archive_decode,
    UmiRadPropertyEditorDescriptor, UmiRadPropertyEditorDescriptorArchiveSchema, UmiRadPropertyEditorDescriptorArchiveBound, UmiRadPropertyEditorDescriptorArchiveWrite, UmiRadPropertyEditorDescriptorArchiveRead, UmiRadPropertyEditorDescriptorArchiveValidate)
