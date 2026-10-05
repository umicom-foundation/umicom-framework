/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/palette_override.c
 *
 * PURPOSE:
 *   Describe a scoped semantic palette override without embedding literal renderer colours.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/palette_override.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_palette_override_init(UmiAppearancePaletteOverride *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->override_id,sizeof item->override_id,"override.accent");
    (void)umi_appearance_copy_text(item->role_id,sizeof item->role_id,"accent.primary");
    (void)umi_appearance_copy_text(item->token_id,sizeof item->token_id,"studio.accent");
    item->scope=UMI_APPEARANCE_SCOPE_APPLICATION;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_palette_override_is_valid(const UmiAppearancePaletteOverride *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->override_id, '\0', sizeof(item->override_id)) == NULL) return 0;
    if (memchr(item->role_id, '\0', sizeof(item->role_id)) == NULL) return 0;
    if (memchr(item->token_id, '\0', sizeof(item->token_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->override_id) && umi_appearance_id_valid(item->role_id) && umi_appearance_id_valid(item->token_id));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearancePaletteOverrideArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x53ac5c4b08268a6c);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearancePaletteOverride *)0)->override_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearancePaletteOverride *)0)->role_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearancePaletteOverride *)0)->token_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearancePaletteOverrideArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearancePaletteOverride *)0)->override_id) - 1U +
        8U + sizeof(((UmiAppearancePaletteOverride *)0)->role_id) - 1U +
        8U + sizeof(((UmiAppearancePaletteOverride *)0)->token_id) - 1U +
        8U;
}
static void UmiAppearancePaletteOverrideArchiveWrite(UmiArchiveWriter *writer, const UmiAppearancePaletteOverride *value)
{
    UmiArchiveWriteText(writer, value->override_id, sizeof(value->override_id));
    UmiArchiveWriteText(writer, value->role_id, sizeof(value->role_id));
    UmiArchiveWriteText(writer, value->token_id, sizeof(value->token_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->scope);
}
static void UmiAppearancePaletteOverrideArchiveRead(UmiArchiveReader *reader, UmiAppearancePaletteOverride *value)
{
    UmiArchiveReadText(reader, value->override_id, sizeof(value->override_id));
    UmiArchiveReadText(reader, value->role_id, sizeof(value->role_id));
    UmiArchiveReadText(reader, value->token_id, sizeof(value->token_id));
    value->scope = (UmiAppearanceScope)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAppearancePaletteOverrideArchiveValidate(const UmiAppearancePaletteOverride *value)
{
    return umi_appearance_palette_override_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_palette_override_archive_encode, umi_appearance_palette_override_archive_decode,
    UmiAppearancePaletteOverride, UmiAppearancePaletteOverrideArchiveSchema, UmiAppearancePaletteOverrideArchiveBound, UmiAppearancePaletteOverrideArchiveWrite, UmiAppearancePaletteOverrideArchiveRead, UmiAppearancePaletteOverrideArchiveValidate)
