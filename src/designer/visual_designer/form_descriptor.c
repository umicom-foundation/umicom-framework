/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/form_descriptor.c
 *
 * PURPOSE:
 *   Describe a form, semantic root and Framework submit command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/form_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer form descriptor from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_form_descriptor_init(UmiRadFormDescriptor *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->form_id, sizeof item->form_id, "form_descriptor");
    (void)umi_rad_copy_text(item->title, sizeof item->title, "form_descriptor");
    (void)umi_rad_copy_text(item->root_component_id, sizeof item->root_component_id, "form_descriptor");
    (void)umi_rad_copy_text(item->submit_command_id, sizeof item->submit_command_id, "form_descriptor");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer form descriptor satisfies its contract before another service relies on
 * it.
 */
int umi_rad_form_descriptor_is_valid(const UmiRadFormDescriptor *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->form_id, '\0', sizeof(item->form_id)) == NULL) return 0;
    if (memchr(item->title, '\0', sizeof(item->title)) == NULL) return 0;
    if (memchr(item->root_component_id, '\0', sizeof(item->root_component_id)) == NULL) return 0;
    if (memchr(item->submit_command_id, '\0', sizeof(item->submit_command_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->form_id) && umi_rad_id_valid(item->root_component_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadFormDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x794f5ef0d9eb9822);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadFormDescriptor *)0)->form_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadFormDescriptor *)0)->title)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadFormDescriptor *)0)->root_component_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadFormDescriptor *)0)->submit_command_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadFormDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadFormDescriptor *)0)->form_id) - 1U +
        8U + sizeof(((UmiRadFormDescriptor *)0)->title) - 1U +
        8U + sizeof(((UmiRadFormDescriptor *)0)->root_component_id) - 1U +
        8U + sizeof(((UmiRadFormDescriptor *)0)->submit_command_id) - 1U;
}
static void UmiRadFormDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiRadFormDescriptor *value)
{
    UmiArchiveWriteText(writer, value->form_id, sizeof(value->form_id));
    UmiArchiveWriteText(writer, value->title, sizeof(value->title));
    UmiArchiveWriteText(writer, value->root_component_id, sizeof(value->root_component_id));
    UmiArchiveWriteText(writer, value->submit_command_id, sizeof(value->submit_command_id));
}
static void UmiRadFormDescriptorArchiveRead(UmiArchiveReader *reader, UmiRadFormDescriptor *value)
{
    UmiArchiveReadText(reader, value->form_id, sizeof(value->form_id));
    UmiArchiveReadText(reader, value->title, sizeof(value->title));
    UmiArchiveReadText(reader, value->root_component_id, sizeof(value->root_component_id));
    UmiArchiveReadText(reader, value->submit_command_id, sizeof(value->submit_command_id));
}
static UmiStatus UmiRadFormDescriptorArchiveValidate(const UmiRadFormDescriptor *value)
{
    return umi_rad_form_descriptor_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_form_descriptor_archive_encode, umi_rad_form_descriptor_archive_decode,
    UmiRadFormDescriptor, UmiRadFormDescriptorArchiveSchema, UmiRadFormDescriptorArchiveBound, UmiRadFormDescriptorArchiveWrite, UmiRadFormDescriptorArchiveRead, UmiRadFormDescriptorArchiveValidate)
