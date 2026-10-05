/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/semantic_palette_map.c
 *
 * PURPOSE:
 *   Map a semantic colour role to a Design-System token identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/semantic_palette_map.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_semantic_palette_map_init(UmiAppearanceSemanticPaletteMap *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->role_id,sizeof item->role_id,"surface.background");
    (void)umi_appearance_copy_text(item->token_id,sizeof item->token_id,"color.surface.background");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_semantic_palette_map_is_valid(const UmiAppearanceSemanticPaletteMap *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->role_id, '\0', sizeof(item->role_id)) == NULL) return 0;
    if (memchr(item->token_id, '\0', sizeof(item->token_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->role_id) && umi_appearance_id_valid(item->token_id));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceSemanticPaletteMapArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x090534590f61ba14);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceSemanticPaletteMap *)0)->role_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceSemanticPaletteMap *)0)->token_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceSemanticPaletteMapArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceSemanticPaletteMap *)0)->role_id) - 1U +
        8U + sizeof(((UmiAppearanceSemanticPaletteMap *)0)->token_id) - 1U;
}
static void UmiAppearanceSemanticPaletteMapArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceSemanticPaletteMap *value)
{
    UmiArchiveWriteText(writer, value->role_id, sizeof(value->role_id));
    UmiArchiveWriteText(writer, value->token_id, sizeof(value->token_id));
}
static void UmiAppearanceSemanticPaletteMapArchiveRead(UmiArchiveReader *reader, UmiAppearanceSemanticPaletteMap *value)
{
    UmiArchiveReadText(reader, value->role_id, sizeof(value->role_id));
    UmiArchiveReadText(reader, value->token_id, sizeof(value->token_id));
}
static UmiStatus UmiAppearanceSemanticPaletteMapArchiveValidate(const UmiAppearanceSemanticPaletteMap *value)
{
    return umi_appearance_semantic_palette_map_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_semantic_palette_map_archive_encode, umi_appearance_semantic_palette_map_archive_decode,
    UmiAppearanceSemanticPaletteMap, UmiAppearanceSemanticPaletteMapArchiveSchema, UmiAppearanceSemanticPaletteMapArchiveBound, UmiAppearanceSemanticPaletteMapArchiveWrite, UmiAppearanceSemanticPaletteMapArchiveRead, UmiAppearanceSemanticPaletteMapArchiveValidate)
