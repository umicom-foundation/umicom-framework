/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/font_family_descriptor.c
 *
 * PURPOSE:
 *   Describe one semantic font-family candidate and its broad typographic classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_family_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_font_family_descriptor_init(UmiAppearanceFontFamilyDescriptor *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->family_id,sizeof item->family_id,"font.ui");
    (void)umi_appearance_copy_text(item->family_name,sizeof item->family_name,"system-ui");
    (void)umi_appearance_copy_text(item->classification,sizeof item->classification,"sans");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_font_family_descriptor_is_valid(const UmiAppearanceFontFamilyDescriptor *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->family_id, '\0', sizeof(item->family_id)) == NULL) return 0;
    if (memchr(item->family_name, '\0', sizeof(item->family_name)) == NULL) return 0;
    if (memchr(item->classification, '\0', sizeof(item->classification)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->family_id) && item->family_name[0] != 0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceFontFamilyDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa2e4017758a009fa);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceFontFamilyDescriptor *)0)->family_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceFontFamilyDescriptor *)0)->family_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceFontFamilyDescriptor *)0)->classification)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceFontFamilyDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceFontFamilyDescriptor *)0)->family_id) - 1U +
        8U + sizeof(((UmiAppearanceFontFamilyDescriptor *)0)->family_name) - 1U +
        8U + sizeof(((UmiAppearanceFontFamilyDescriptor *)0)->classification) - 1U +
        8U +
        8U;
}
static void UmiAppearanceFontFamilyDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceFontFamilyDescriptor *value)
{
    UmiArchiveWriteText(writer, value->family_id, sizeof(value->family_id));
    UmiArchiveWriteText(writer, value->family_name, sizeof(value->family_name));
    UmiArchiveWriteText(writer, value->classification, sizeof(value->classification));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->monospace);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->variable_font);
}
static void UmiAppearanceFontFamilyDescriptorArchiveRead(UmiArchiveReader *reader, UmiAppearanceFontFamilyDescriptor *value)
{
    UmiArchiveReadText(reader, value->family_id, sizeof(value->family_id));
    UmiArchiveReadText(reader, value->family_name, sizeof(value->family_name));
    UmiArchiveReadText(reader, value->classification, sizeof(value->classification));
    value->monospace = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->variable_font = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceFontFamilyDescriptorArchiveValidate(const UmiAppearanceFontFamilyDescriptor *value)
{
    return umi_appearance_font_family_descriptor_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_font_family_descriptor_archive_encode, umi_appearance_font_family_descriptor_archive_decode,
    UmiAppearanceFontFamilyDescriptor, UmiAppearanceFontFamilyDescriptorArchiveSchema, UmiAppearanceFontFamilyDescriptorArchiveBound, UmiAppearanceFontFamilyDescriptorArchiveWrite, UmiAppearanceFontFamilyDescriptorArchiveRead, UmiAppearanceFontFamilyDescriptorArchiveValidate)
