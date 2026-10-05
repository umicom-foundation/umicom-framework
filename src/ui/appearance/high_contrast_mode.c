/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/high_contrast_mode.c
 *
 * PURPOSE:
 *   Represent high-contrast presentation requirements layered over the canonical Design System.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/high_contrast_mode.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_high_contrast_mode_init(UmiAppearanceHighContrastMode *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->mode_id,sizeof item->mode_id,"high-contrast");
    item->enabled=true;
    item->force_visible_borders=true;
    item->force_focus_outline=true;
    item->minimum_border_width=2.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_high_contrast_mode_is_valid(const UmiAppearanceHighContrastMode *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->mode_id, '\0', sizeof(item->mode_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->mode_id) && item->minimum_border_width >= 0.0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceHighContrastModeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdb2449b5acbcb96a);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceHighContrastMode *)0)->mode_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceHighContrastModeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceHighContrastMode *)0)->mode_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceHighContrastModeArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceHighContrastMode *value)
{
    UmiArchiveWriteText(writer, value->mode_id, sizeof(value->mode_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->force_visible_borders);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->force_focus_outline);
    UmiArchiveWriteDouble(writer, value->minimum_border_width);
}
static void UmiAppearanceHighContrastModeArchiveRead(UmiArchiveReader *reader, UmiAppearanceHighContrastMode *value)
{
    UmiArchiveReadText(reader, value->mode_id, sizeof(value->mode_id));
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->force_visible_borders = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->force_focus_outline = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->minimum_border_width = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAppearanceHighContrastModeArchiveValidate(const UmiAppearanceHighContrastMode *value)
{
    return umi_appearance_high_contrast_mode_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_high_contrast_mode_archive_encode, umi_appearance_high_contrast_mode_archive_decode,
    UmiAppearanceHighContrastMode, UmiAppearanceHighContrastModeArchiveSchema, UmiAppearanceHighContrastModeArchiveBound, UmiAppearanceHighContrastModeArchiveWrite, UmiAppearanceHighContrastModeArchiveRead, UmiAppearanceHighContrastModeArchiveValidate)
