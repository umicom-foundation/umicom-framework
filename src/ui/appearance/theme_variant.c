/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/theme_variant.c
 *
 * PURPOSE:
 *   Bind a semantic theme pack to light, dark or high-contrast presentation mode.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_variant.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_theme_variant_init(UmiAppearanceThemeVariant *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->variant_id,sizeof item->variant_id,"variant.dark");
    (void)umi_appearance_copy_text(item->pack_id,sizeof item->pack_id,"theme.default.dark");
    item->mode=UMI_DESIGN_THEME_DARK;
    item->preferred=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_theme_variant_is_valid(const UmiAppearanceThemeVariant *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->variant_id, '\0', sizeof(item->variant_id)) == NULL) return 0;
    if (memchr(item->pack_id, '\0', sizeof(item->pack_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->variant_id) && umi_appearance_id_valid(item->pack_id));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceThemeVariantArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd8486ac4df661662);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceThemeVariant *)0)->variant_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceThemeVariant *)0)->pack_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceThemeVariantArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceThemeVariant *)0)->variant_id) - 1U +
        8U + sizeof(((UmiAppearanceThemeVariant *)0)->pack_id) - 1U +
        8U +
        8U;
}
static void UmiAppearanceThemeVariantArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceThemeVariant *value)
{
    UmiArchiveWriteText(writer, value->variant_id, sizeof(value->variant_id));
    UmiArchiveWriteText(writer, value->pack_id, sizeof(value->pack_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->preferred);
}
static void UmiAppearanceThemeVariantArchiveRead(UmiArchiveReader *reader, UmiAppearanceThemeVariant *value)
{
    UmiArchiveReadText(reader, value->variant_id, sizeof(value->variant_id));
    UmiArchiveReadText(reader, value->pack_id, sizeof(value->pack_id));
    value->mode = (UmiDesignThemeMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->preferred = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceThemeVariantArchiveValidate(const UmiAppearanceThemeVariant *value)
{
    return umi_appearance_theme_variant_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_theme_variant_archive_encode, umi_appearance_theme_variant_archive_decode,
    UmiAppearanceThemeVariant, UmiAppearanceThemeVariantArchiveSchema, UmiAppearanceThemeVariantArchiveBound, UmiAppearanceThemeVariantArchiveWrite, UmiAppearanceThemeVariantArchiveRead, UmiAppearanceThemeVariantArchiveValidate)
