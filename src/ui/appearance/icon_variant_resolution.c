/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/icon_variant_resolution.c
 *
 * PURPOSE:
 *   Resolve light/dark/high-contrast and direction-aware icon variants while preserving semantic identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_variant_resolution.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_icon_variant_resolution_init(UmiAppearanceIconVariantResolution *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->icon_id,sizeof item->icon_id,"navigation.forward");
    (void)umi_appearance_copy_text(item->resolved_variant_id,sizeof item->resolved_variant_id,"navigation.forward.dark");
    item->mode=UMI_DESIGN_THEME_DARK;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_icon_variant_resolution_is_valid(const UmiAppearanceIconVariantResolution *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->icon_id, '\0', sizeof(item->icon_id)) == NULL) return 0;
    if (memchr(item->resolved_variant_id, '\0', sizeof(item->resolved_variant_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->icon_id) && umi_appearance_id_valid(item->resolved_variant_id));
}
/*
 * Provide the appearance icon variant resolution set direction operation used by this
 * module and its client applications.
 */
void umi_appearance_icon_variant_resolution_set_direction(UmiAppearanceIconVariantResolution *item,int rtl,int direction_sensitive){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item!=NULL){item->rtl=rtl!=0;item->mirrored=item->rtl&&direction_sensitive;}}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceIconVariantResolutionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc47622853457a114);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceIconVariantResolution *)0)->icon_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceIconVariantResolution *)0)->resolved_variant_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceIconVariantResolutionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceIconVariantResolution *)0)->icon_id) - 1U +
        8U + sizeof(((UmiAppearanceIconVariantResolution *)0)->resolved_variant_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceIconVariantResolutionArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceIconVariantResolution *value)
{
    UmiArchiveWriteText(writer, value->icon_id, sizeof(value->icon_id));
    UmiArchiveWriteText(writer, value->resolved_variant_id, sizeof(value->resolved_variant_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->rtl);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->mirrored);
}
static void UmiAppearanceIconVariantResolutionArchiveRead(UmiArchiveReader *reader, UmiAppearanceIconVariantResolution *value)
{
    UmiArchiveReadText(reader, value->icon_id, sizeof(value->icon_id));
    UmiArchiveReadText(reader, value->resolved_variant_id, sizeof(value->resolved_variant_id));
    value->mode = (UmiDesignThemeMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->rtl = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->mirrored = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceIconVariantResolutionArchiveValidate(const UmiAppearanceIconVariantResolution *value)
{
    return umi_appearance_icon_variant_resolution_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_icon_variant_resolution_archive_encode, umi_appearance_icon_variant_resolution_archive_decode,
    UmiAppearanceIconVariantResolution, UmiAppearanceIconVariantResolutionArchiveSchema, UmiAppearanceIconVariantResolutionArchiveBound, UmiAppearanceIconVariantResolutionArchiveWrite, UmiAppearanceIconVariantResolutionArchiveRead, UmiAppearanceIconVariantResolutionArchiveValidate)
