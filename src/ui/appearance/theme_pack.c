/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/theme_pack.c
 *
 * PURPOSE:
 *   Describe a versionable semantic theme pack without toolkit CSS or widget classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_pack.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_theme_pack_init(UmiAppearanceThemePack *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->pack_id, sizeof item->pack_id, "theme.default.dark");
    (void)umi_appearance_copy_text(item->brand_id, sizeof item->brand_id, "umicom");
    (void)umi_appearance_copy_text(item->token_set_id, sizeof item->token_set_id, "design.tokens.default");
    item->mode = UMI_DESIGN_THEME_DARK;
    item->revision = 1U;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_theme_pack_is_valid(const UmiAppearanceThemePack *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->pack_id, '\0', sizeof(item->pack_id)) == NULL) return 0;
    if (memchr(item->parent_pack_id, '\0', sizeof(item->parent_pack_id)) == NULL) return 0;
    if (memchr(item->brand_id, '\0', sizeof(item->brand_id)) == NULL) return 0;
    if (memchr(item->token_set_id, '\0', sizeof(item->token_set_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->pack_id) && umi_appearance_id_valid(item->token_set_id) && item->revision > 0U);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceThemePackArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x507fcafa84ab43dd);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceThemePack *)0)->pack_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceThemePack *)0)->parent_pack_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceThemePack *)0)->brand_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceThemePack *)0)->token_set_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceThemePackArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceThemePack *)0)->pack_id) - 1U +
        8U + sizeof(((UmiAppearanceThemePack *)0)->parent_pack_id) - 1U +
        8U + sizeof(((UmiAppearanceThemePack *)0)->brand_id) - 1U +
        8U + sizeof(((UmiAppearanceThemePack *)0)->token_set_id) - 1U +
        8U +
        8U;
}
static void UmiAppearanceThemePackArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceThemePack *value)
{
    UmiArchiveWriteText(writer, value->pack_id, sizeof(value->pack_id));
    UmiArchiveWriteText(writer, value->parent_pack_id, sizeof(value->parent_pack_id));
    UmiArchiveWriteText(writer, value->brand_id, sizeof(value->brand_id));
    UmiArchiveWriteText(writer, value->token_set_id, sizeof(value->token_set_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiAppearanceThemePackArchiveRead(UmiArchiveReader *reader, UmiAppearanceThemePack *value)
{
    UmiArchiveReadText(reader, value->pack_id, sizeof(value->pack_id));
    UmiArchiveReadText(reader, value->parent_pack_id, sizeof(value->parent_pack_id));
    UmiArchiveReadText(reader, value->brand_id, sizeof(value->brand_id));
    UmiArchiveReadText(reader, value->token_set_id, sizeof(value->token_set_id));
    value->mode = (UmiDesignThemeMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiAppearanceThemePackArchiveValidate(const UmiAppearanceThemePack *value)
{
    return umi_appearance_theme_pack_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_theme_pack_archive_encode, umi_appearance_theme_pack_archive_decode,
    UmiAppearanceThemePack, UmiAppearanceThemePackArchiveSchema, UmiAppearanceThemePackArchiveBound, UmiAppearanceThemePackArchiveWrite, UmiAppearanceThemePackArchiveRead, UmiAppearanceThemePackArchiveValidate)
