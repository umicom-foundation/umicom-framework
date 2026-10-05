/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/event_descriptor.c
 *
 * PURPOSE:
 *   Describe an event exposed by a semantic component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/event_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer event descriptor from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_event_descriptor_init(UmiRadEventDescriptor *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->event_id, sizeof item->event_id, "event_descriptor");
    (void)umi_rad_copy_text(item->label, sizeof item->label, "event_descriptor");
    (void)umi_rad_copy_text(item->parameter_type, sizeof item->parameter_type, "event_descriptor");
    item->bindable = true;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer event descriptor satisfies its contract before another service relies on
 * it.
 */
int umi_rad_event_descriptor_is_valid(const UmiRadEventDescriptor *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->event_id, '\0', sizeof(item->event_id)) == NULL) return 0;
    if (memchr(item->label, '\0', sizeof(item->label)) == NULL) return 0;
    if (memchr(item->parameter_type, '\0', sizeof(item->parameter_type)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->event_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadEventDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4c53a44fed2fea14);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadEventDescriptor *)0)->event_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadEventDescriptor *)0)->label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadEventDescriptor *)0)->parameter_type)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadEventDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadEventDescriptor *)0)->event_id) - 1U +
        8U + sizeof(((UmiRadEventDescriptor *)0)->label) - 1U +
        8U + sizeof(((UmiRadEventDescriptor *)0)->parameter_type) - 1U +
        8U;
}
static void UmiRadEventDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiRadEventDescriptor *value)
{
    UmiArchiveWriteText(writer, value->event_id, sizeof(value->event_id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteText(writer, value->parameter_type, sizeof(value->parameter_type));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->bindable);
}
static void UmiRadEventDescriptorArchiveRead(UmiArchiveReader *reader, UmiRadEventDescriptor *value)
{
    UmiArchiveReadText(reader, value->event_id, sizeof(value->event_id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    UmiArchiveReadText(reader, value->parameter_type, sizeof(value->parameter_type));
    value->bindable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadEventDescriptorArchiveValidate(const UmiRadEventDescriptor *value)
{
    return umi_rad_event_descriptor_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_event_descriptor_archive_encode, umi_rad_event_descriptor_archive_decode,
    UmiRadEventDescriptor, UmiRadEventDescriptorArchiveSchema, UmiRadEventDescriptorArchiveBound, UmiRadEventDescriptorArchiveWrite, UmiRadEventDescriptorArchiveRead, UmiRadEventDescriptorArchiveValidate)
