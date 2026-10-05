/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/surface_semantics.c
 *
 * PURPOSE:
 *   Describe semantic surface hierarchy and elevation intent independently of renderer primitives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/surface_semantics.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_surface_semantics_init(UmiAppearanceSurfaceSemantics *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->surface_id,sizeof item->surface_id,"surface.panel");
    (void)umi_appearance_copy_text(item->background_role,sizeof item->background_role,"surface.panel");
    (void)umi_appearance_copy_text(item->foreground_role,sizeof item->foreground_role,"text.primary");
    item->elevation_level=1;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_surface_semantics_is_valid(const UmiAppearanceSurfaceSemantics *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->surface_id, '\0', sizeof(item->surface_id)) == NULL) return 0;
    if (memchr(item->background_role, '\0', sizeof(item->background_role)) == NULL) return 0;
    if (memchr(item->foreground_role, '\0', sizeof(item->foreground_role)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->surface_id) && item->elevation_level >= 0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceSurfaceSemanticsArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x52e8d630da524a18);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceSurfaceSemantics *)0)->surface_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceSurfaceSemantics *)0)->background_role)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceSurfaceSemantics *)0)->foreground_role)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceSurfaceSemanticsArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceSurfaceSemantics *)0)->surface_id) - 1U +
        8U + sizeof(((UmiAppearanceSurfaceSemantics *)0)->background_role) - 1U +
        8U + sizeof(((UmiAppearanceSurfaceSemantics *)0)->foreground_role) - 1U +
        8U;
}
static void UmiAppearanceSurfaceSemanticsArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceSurfaceSemantics *value)
{
    UmiArchiveWriteText(writer, value->surface_id, sizeof(value->surface_id));
    UmiArchiveWriteText(writer, value->background_role, sizeof(value->background_role));
    UmiArchiveWriteText(writer, value->foreground_role, sizeof(value->foreground_role));
    UmiArchiveWriteSigned(writer, (int64_t)value->elevation_level);
}
static void UmiAppearanceSurfaceSemanticsArchiveRead(UmiArchiveReader *reader, UmiAppearanceSurfaceSemantics *value)
{
    UmiArchiveReadText(reader, value->surface_id, sizeof(value->surface_id));
    UmiArchiveReadText(reader, value->background_role, sizeof(value->background_role));
    UmiArchiveReadText(reader, value->foreground_role, sizeof(value->foreground_role));
    value->elevation_level = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiAppearanceSurfaceSemanticsArchiveValidate(const UmiAppearanceSurfaceSemantics *value)
{
    return umi_appearance_surface_semantics_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_surface_semantics_archive_encode, umi_appearance_surface_semantics_archive_decode,
    UmiAppearanceSurfaceSemantics, UmiAppearanceSurfaceSemanticsArchiveSchema, UmiAppearanceSurfaceSemanticsArchiveBound, UmiAppearanceSurfaceSemanticsArchiveWrite, UmiAppearanceSurfaceSemanticsArchiveRead, UmiAppearanceSurfaceSemanticsArchiveValidate)
