/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/surface_style_projection.c
 *
 * PURPOSE:
 *   Resolve semantic surface roles to token identities consumed by frontend renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/surface_style_projection.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_surface_style_projection_init(UmiAppearanceSurfaceStyleProjection *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->surface_id,sizeof item->surface_id,"surface.panel");
    (void)umi_appearance_copy_text(item->background_token,sizeof item->background_token,"color.surface.panel");
    (void)umi_appearance_copy_text(item->foreground_token,sizeof item->foreground_token,"color.text.primary");
    (void)umi_appearance_copy_text(item->border_token,sizeof item->border_token,"color.border.subtle");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_surface_style_projection_is_valid(const UmiAppearanceSurfaceStyleProjection *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->surface_id, '\0', sizeof(item->surface_id)) == NULL) return 0;
    if (memchr(item->background_token, '\0', sizeof(item->background_token)) == NULL) return 0;
    if (memchr(item->foreground_token, '\0', sizeof(item->foreground_token)) == NULL) return 0;
    if (memchr(item->border_token, '\0', sizeof(item->border_token)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->surface_id) && umi_appearance_id_valid(item->background_token) && umi_appearance_id_valid(item->foreground_token));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceSurfaceStyleProjectionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x41374bd4bc7bd2e9);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceSurfaceStyleProjection *)0)->surface_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceSurfaceStyleProjection *)0)->background_token)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceSurfaceStyleProjection *)0)->foreground_token)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceSurfaceStyleProjection *)0)->border_token)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceSurfaceStyleProjectionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceSurfaceStyleProjection *)0)->surface_id) - 1U +
        8U + sizeof(((UmiAppearanceSurfaceStyleProjection *)0)->background_token) - 1U +
        8U + sizeof(((UmiAppearanceSurfaceStyleProjection *)0)->foreground_token) - 1U +
        8U + sizeof(((UmiAppearanceSurfaceStyleProjection *)0)->border_token) - 1U;
}
static void UmiAppearanceSurfaceStyleProjectionArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceSurfaceStyleProjection *value)
{
    UmiArchiveWriteText(writer, value->surface_id, sizeof(value->surface_id));
    UmiArchiveWriteText(writer, value->background_token, sizeof(value->background_token));
    UmiArchiveWriteText(writer, value->foreground_token, sizeof(value->foreground_token));
    UmiArchiveWriteText(writer, value->border_token, sizeof(value->border_token));
}
static void UmiAppearanceSurfaceStyleProjectionArchiveRead(UmiArchiveReader *reader, UmiAppearanceSurfaceStyleProjection *value)
{
    UmiArchiveReadText(reader, value->surface_id, sizeof(value->surface_id));
    UmiArchiveReadText(reader, value->background_token, sizeof(value->background_token));
    UmiArchiveReadText(reader, value->foreground_token, sizeof(value->foreground_token));
    UmiArchiveReadText(reader, value->border_token, sizeof(value->border_token));
}
static UmiStatus UmiAppearanceSurfaceStyleProjectionArchiveValidate(const UmiAppearanceSurfaceStyleProjection *value)
{
    return umi_appearance_surface_style_projection_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_surface_style_projection_archive_encode, umi_appearance_surface_style_projection_archive_decode,
    UmiAppearanceSurfaceStyleProjection, UmiAppearanceSurfaceStyleProjectionArchiveSchema, UmiAppearanceSurfaceStyleProjectionArchiveBound, UmiAppearanceSurfaceStyleProjectionArchiveWrite, UmiAppearanceSurfaceStyleProjectionArchiveRead, UmiAppearanceSurfaceStyleProjectionArchiveValidate)
