/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/border_style_projection.c
 *
 * PURPOSE:
 *   Resolve semantic border width, radius and token identity without exposing toolkit CSS syntax.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/border_style_projection.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_border_style_projection_init(UmiAppearanceBorderStyleProjection *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->style_id,sizeof item->style_id,"border.panel");
    (void)umi_appearance_copy_text(item->color_token,sizeof item->color_token,"color.border.subtle");
    item->width_dp=1.0;
    item->radius_dp=6.0;
    item->visible=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_border_style_projection_is_valid(const UmiAppearanceBorderStyleProjection *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->style_id, '\0', sizeof(item->style_id)) == NULL) return 0;
    if (memchr(item->color_token, '\0', sizeof(item->color_token)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->style_id) && item->width_dp >= 0.0 && item->radius_dp >= 0.0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceBorderStyleProjectionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x38f5c3c653cb6398);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceBorderStyleProjection *)0)->style_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceBorderStyleProjection *)0)->color_token)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceBorderStyleProjectionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceBorderStyleProjection *)0)->style_id) - 1U +
        8U + sizeof(((UmiAppearanceBorderStyleProjection *)0)->color_token) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceBorderStyleProjectionArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceBorderStyleProjection *value)
{
    UmiArchiveWriteText(writer, value->style_id, sizeof(value->style_id));
    UmiArchiveWriteText(writer, value->color_token, sizeof(value->color_token));
    UmiArchiveWriteDouble(writer, value->width_dp);
    UmiArchiveWriteDouble(writer, value->radius_dp);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible);
}
static void UmiAppearanceBorderStyleProjectionArchiveRead(UmiArchiveReader *reader, UmiAppearanceBorderStyleProjection *value)
{
    UmiArchiveReadText(reader, value->style_id, sizeof(value->style_id));
    UmiArchiveReadText(reader, value->color_token, sizeof(value->color_token));
    value->width_dp = UmiArchiveReadDouble(reader);
    value->radius_dp = UmiArchiveReadDouble(reader);
    value->visible = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceBorderStyleProjectionArchiveValidate(const UmiAppearanceBorderStyleProjection *value)
{
    return umi_appearance_border_style_projection_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_border_style_projection_archive_encode, umi_appearance_border_style_projection_archive_decode,
    UmiAppearanceBorderStyleProjection, UmiAppearanceBorderStyleProjectionArchiveSchema, UmiAppearanceBorderStyleProjectionArchiveBound, UmiAppearanceBorderStyleProjectionArchiveWrite, UmiAppearanceBorderStyleProjectionArchiveRead, UmiAppearanceBorderStyleProjectionArchiveValidate)
