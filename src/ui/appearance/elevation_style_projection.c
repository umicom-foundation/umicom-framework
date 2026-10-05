/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/elevation_style_projection.c
 *
 * PURPOSE:
 *   Resolve semantic elevation levels to shadow and border tokens suitable for each frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/elevation_style_projection.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_elevation_style_projection_init(UmiAppearanceElevationStyleProjection *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->style_id,sizeof item->style_id,"elevation.panel");
    item->elevation_level=2;
    (void)umi_appearance_copy_text(item->shadow_token,sizeof item->shadow_token,"shadow.level2");
    (void)umi_appearance_copy_text(item->fallback_border_token,sizeof item->fallback_border_token,"color.border.strong");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_elevation_style_projection_is_valid(const UmiAppearanceElevationStyleProjection *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->style_id, '\0', sizeof(item->style_id)) == NULL) return 0;
    if (memchr(item->shadow_token, '\0', sizeof(item->shadow_token)) == NULL) return 0;
    if (memchr(item->fallback_border_token, '\0', sizeof(item->fallback_border_token)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->style_id) && item->elevation_level >= 0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceElevationStyleProjectionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5ff65cdc6d41b2c2);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceElevationStyleProjection *)0)->style_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceElevationStyleProjection *)0)->shadow_token)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceElevationStyleProjection *)0)->fallback_border_token)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceElevationStyleProjectionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceElevationStyleProjection *)0)->style_id) - 1U +
        8U +
        8U + sizeof(((UmiAppearanceElevationStyleProjection *)0)->shadow_token) - 1U +
        8U + sizeof(((UmiAppearanceElevationStyleProjection *)0)->fallback_border_token) - 1U;
}
static void UmiAppearanceElevationStyleProjectionArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceElevationStyleProjection *value)
{
    UmiArchiveWriteText(writer, value->style_id, sizeof(value->style_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->elevation_level);
    UmiArchiveWriteText(writer, value->shadow_token, sizeof(value->shadow_token));
    UmiArchiveWriteText(writer, value->fallback_border_token, sizeof(value->fallback_border_token));
}
static void UmiAppearanceElevationStyleProjectionArchiveRead(UmiArchiveReader *reader, UmiAppearanceElevationStyleProjection *value)
{
    UmiArchiveReadText(reader, value->style_id, sizeof(value->style_id));
    value->elevation_level = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    UmiArchiveReadText(reader, value->shadow_token, sizeof(value->shadow_token));
    UmiArchiveReadText(reader, value->fallback_border_token, sizeof(value->fallback_border_token));
}
static UmiStatus UmiAppearanceElevationStyleProjectionArchiveValidate(const UmiAppearanceElevationStyleProjection *value)
{
    return umi_appearance_elevation_style_projection_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_elevation_style_projection_archive_encode, umi_appearance_elevation_style_projection_archive_decode,
    UmiAppearanceElevationStyleProjection, UmiAppearanceElevationStyleProjectionArchiveSchema, UmiAppearanceElevationStyleProjectionArchiveBound, UmiAppearanceElevationStyleProjectionArchiveWrite, UmiAppearanceElevationStyleProjectionArchiveRead, UmiAppearanceElevationStyleProjectionArchiveValidate)
