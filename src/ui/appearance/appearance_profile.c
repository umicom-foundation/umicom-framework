/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/appearance_profile.c
 *
 * PURPOSE:
 *   Capture a resolved user/application appearance profile shared by every renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_profile.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_profile_init(UmiAppearanceAppearanceProfile *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->profile_id, sizeof item->profile_id, "appearance.default");
    item->theme_mode = UMI_DESIGN_THEME_DARK;
    item->density = UMI_DESIGN_DENSITY_STANDARD;
    item->text_scale = 1.0;
    item->display_scale = 1.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_profile_is_valid(const UmiAppearanceAppearanceProfile *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->profile_id, '\0', sizeof(item->profile_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->profile_id) && item->text_scale > 0.0 && item->display_scale > 0.0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceAppearanceProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x65d6cc504cce8304);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceAppearanceProfile *)0)->profile_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceAppearanceProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceAppearanceProfile *)0)->profile_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceAppearanceProfileArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceAppearanceProfile *value)
{
    UmiArchiveWriteText(writer, value->profile_id, sizeof(value->profile_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->theme_mode);
    UmiArchiveWriteSigned(writer, (int64_t)value->density);
    UmiArchiveWriteDouble(writer, value->text_scale);
    UmiArchiveWriteDouble(writer, value->display_scale);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->reduced_motion);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->high_contrast);
}
static void UmiAppearanceAppearanceProfileArchiveRead(UmiArchiveReader *reader, UmiAppearanceAppearanceProfile *value)
{
    UmiArchiveReadText(reader, value->profile_id, sizeof(value->profile_id));
    value->theme_mode = (UmiDesignThemeMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->density = (UmiDesignDensity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->text_scale = UmiArchiveReadDouble(reader);
    value->display_scale = UmiArchiveReadDouble(reader);
    value->reduced_motion = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->high_contrast = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceAppearanceProfileArchiveValidate(const UmiAppearanceAppearanceProfile *value)
{
    return umi_appearance_profile_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_profile_archive_encode, umi_appearance_profile_archive_decode,
    UmiAppearanceAppearanceProfile, UmiAppearanceAppearanceProfileArchiveSchema, UmiAppearanceAppearanceProfileArchiveBound, UmiAppearanceAppearanceProfileArchiveWrite, UmiAppearanceAppearanceProfileArchiveRead, UmiAppearanceAppearanceProfileArchiveValidate)
