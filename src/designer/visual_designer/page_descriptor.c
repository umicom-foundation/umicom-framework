/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/page_descriptor.c
 *
 * PURPOSE:
 *   Describe a visual application page, route and semantic root component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/page_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer page descriptor from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_page_descriptor_init(UmiRadPageDescriptor *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->page_id, sizeof item->page_id, "page_descriptor");
    (void)umi_rad_copy_text(item->route, sizeof item->route, "page_descriptor");
    (void)umi_rad_copy_text(item->title, sizeof item->title, "page_descriptor");
    (void)umi_rad_copy_text(item->root_component_id, sizeof item->root_component_id, "page_descriptor");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer page descriptor satisfies its contract before another service relies on
 * it.
 */
int umi_rad_page_descriptor_is_valid(const UmiRadPageDescriptor *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->page_id, '\0', sizeof(item->page_id)) == NULL) return 0;
    if (memchr(item->route, '\0', sizeof(item->route)) == NULL) return 0;
    if (memchr(item->title, '\0', sizeof(item->title)) == NULL) return 0;
    if (memchr(item->root_component_id, '\0', sizeof(item->root_component_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->page_id) && item->route[0] != '\0' && umi_rad_id_valid(item->root_component_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadPageDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd48855b8c07e51ab);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPageDescriptor *)0)->page_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPageDescriptor *)0)->route)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPageDescriptor *)0)->title)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPageDescriptor *)0)->root_component_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadPageDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadPageDescriptor *)0)->page_id) - 1U +
        8U + sizeof(((UmiRadPageDescriptor *)0)->route) - 1U +
        8U + sizeof(((UmiRadPageDescriptor *)0)->title) - 1U +
        8U + sizeof(((UmiRadPageDescriptor *)0)->root_component_id) - 1U;
}
static void UmiRadPageDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiRadPageDescriptor *value)
{
    UmiArchiveWriteText(writer, value->page_id, sizeof(value->page_id));
    UmiArchiveWriteText(writer, value->route, sizeof(value->route));
    UmiArchiveWriteText(writer, value->title, sizeof(value->title));
    UmiArchiveWriteText(writer, value->root_component_id, sizeof(value->root_component_id));
}
static void UmiRadPageDescriptorArchiveRead(UmiArchiveReader *reader, UmiRadPageDescriptor *value)
{
    UmiArchiveReadText(reader, value->page_id, sizeof(value->page_id));
    UmiArchiveReadText(reader, value->route, sizeof(value->route));
    UmiArchiveReadText(reader, value->title, sizeof(value->title));
    UmiArchiveReadText(reader, value->root_component_id, sizeof(value->root_component_id));
}
static UmiStatus UmiRadPageDescriptorArchiveValidate(const UmiRadPageDescriptor *value)
{
    return umi_rad_page_descriptor_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_page_descriptor_archive_encode, umi_rad_page_descriptor_archive_decode,
    UmiRadPageDescriptor, UmiRadPageDescriptorArchiveSchema, UmiRadPageDescriptorArchiveBound, UmiRadPageDescriptorArchiveWrite, UmiRadPageDescriptorArchiveRead, UmiRadPageDescriptorArchiveValidate)
