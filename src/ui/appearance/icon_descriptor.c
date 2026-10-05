/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/icon_descriptor.c
 *
 * PURPOSE:
 *   Describe a semantic icon identity, directionality and scalable/symbolic capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_icon_descriptor_init(UmiAppearanceIconDescriptor *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->icon_id,sizeof item->icon_id,"action.save");
    (void)umi_appearance_copy_text(item->semantic_role,sizeof item->semantic_role,"action.save");
    item->scalable=true;
    item->symbolic=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_icon_descriptor_is_valid(const UmiAppearanceIconDescriptor *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->icon_id, '\0', sizeof(item->icon_id)) == NULL) return 0;
    if (memchr(item->semantic_role, '\0', sizeof(item->semantic_role)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->icon_id) && umi_appearance_id_valid(item->semantic_role));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceIconDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xcd19e8ee8806cb95);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceIconDescriptor *)0)->icon_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceIconDescriptor *)0)->semantic_role)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceIconDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceIconDescriptor *)0)->icon_id) - 1U +
        8U + sizeof(((UmiAppearanceIconDescriptor *)0)->semantic_role) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceIconDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceIconDescriptor *value)
{
    UmiArchiveWriteText(writer, value->icon_id, sizeof(value->icon_id));
    UmiArchiveWriteText(writer, value->semantic_role, sizeof(value->semantic_role));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->scalable);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->symbolic);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->direction_sensitive);
}
static void UmiAppearanceIconDescriptorArchiveRead(UmiArchiveReader *reader, UmiAppearanceIconDescriptor *value)
{
    UmiArchiveReadText(reader, value->icon_id, sizeof(value->icon_id));
    UmiArchiveReadText(reader, value->semantic_role, sizeof(value->semantic_role));
    value->scalable = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->symbolic = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->direction_sensitive = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceIconDescriptorArchiveValidate(const UmiAppearanceIconDescriptor *value)
{
    return umi_appearance_icon_descriptor_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_icon_descriptor_archive_encode, umi_appearance_icon_descriptor_archive_decode,
    UmiAppearanceIconDescriptor, UmiAppearanceIconDescriptorArchiveSchema, UmiAppearanceIconDescriptorArchiveBound, UmiAppearanceIconDescriptorArchiveWrite, UmiAppearanceIconDescriptorArchiveRead, UmiAppearanceIconDescriptorArchiveValidate)
