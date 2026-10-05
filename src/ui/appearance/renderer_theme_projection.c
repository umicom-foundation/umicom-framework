/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/renderer_theme_projection.c
 *
 * PURPOSE:
 *   Record one renderer-specific projection of a semantic style packet without transferring state ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/renderer_theme_projection.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_renderer_theme_projection_init(UmiAppearanceRendererThemeProjection *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->projection_id,sizeof item->projection_id,"projection.gtk4");
    (void)umi_appearance_copy_text(item->packet_id,sizeof item->packet_id,"packet.default");
    item->renderer=UMI_APPEARANCE_RENDERER_GTK4;
    item->semantic_revision=1U;
    item->projected_revision=1U;
    item->complete=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_renderer_theme_projection_is_valid(const UmiAppearanceRendererThemeProjection *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->projection_id, '\0', sizeof(item->projection_id)) == NULL) return 0;
    if (memchr(item->packet_id, '\0', sizeof(item->packet_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->projection_id) && umi_appearance_id_valid(item->packet_id) && item->projected_revision <= item->semantic_revision && item->projected_revision > 0U);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceRendererThemeProjectionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8ae4ebff71ae51f6);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceRendererThemeProjection *)0)->projection_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceRendererThemeProjection *)0)->packet_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceRendererThemeProjectionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceRendererThemeProjection *)0)->projection_id) - 1U +
        8U + sizeof(((UmiAppearanceRendererThemeProjection *)0)->packet_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceRendererThemeProjectionArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceRendererThemeProjection *value)
{
    UmiArchiveWriteText(writer, value->projection_id, sizeof(value->projection_id));
    UmiArchiveWriteText(writer, value->packet_id, sizeof(value->packet_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->renderer);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->semantic_revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->projected_revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->complete);
}
static void UmiAppearanceRendererThemeProjectionArchiveRead(UmiArchiveReader *reader, UmiAppearanceRendererThemeProjection *value)
{
    UmiArchiveReadText(reader, value->projection_id, sizeof(value->projection_id));
    UmiArchiveReadText(reader, value->packet_id, sizeof(value->packet_id));
    value->renderer = (UmiAppearanceRendererKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->semantic_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->projected_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->complete = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceRendererThemeProjectionArchiveValidate(const UmiAppearanceRendererThemeProjection *value)
{
    return umi_appearance_renderer_theme_projection_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_renderer_theme_projection_archive_encode, umi_appearance_renderer_theme_projection_archive_decode,
    UmiAppearanceRendererThemeProjection, UmiAppearanceRendererThemeProjectionArchiveSchema, UmiAppearanceRendererThemeProjectionArchiveBound, UmiAppearanceRendererThemeProjectionArchiveWrite, UmiAppearanceRendererThemeProjectionArchiveRead, UmiAppearanceRendererThemeProjectionArchiveValidate)
