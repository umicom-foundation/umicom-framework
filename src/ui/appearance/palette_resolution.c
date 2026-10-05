/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/palette_resolution.c
 *
 * PURPOSE:
 *   Record the winning token for a semantic palette role after scope precedence is applied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/palette_resolution.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_palette_resolution_init(UmiAppearancePaletteResolution *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->role_id,sizeof item->role_id,"accent.primary");
    (void)umi_appearance_copy_text(item->base_token_id,sizeof item->base_token_id,"accent.default");
    (void)umi_appearance_copy_text(item->resolved_token_id,sizeof item->resolved_token_id,"accent.default");
    item->winning_scope=UMI_APPEARANCE_SCOPE_SYSTEM;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_palette_resolution_is_valid(const UmiAppearancePaletteResolution *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->role_id, '\0', sizeof(item->role_id)) == NULL) return 0;
    if (memchr(item->base_token_id, '\0', sizeof(item->base_token_id)) == NULL) return 0;
    if (memchr(item->resolved_token_id, '\0', sizeof(item->resolved_token_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->role_id) && umi_appearance_id_valid(item->resolved_token_id));
}
/*
 * Provide the appearance palette resolution override operation used by this module and its
 * client applications.
 */
UmiStatus umi_appearance_palette_resolution_override(UmiAppearancePaletteResolution *item,const char *token_id,UmiAppearanceScope scope){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||!umi_appearance_id_valid(token_id))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_copy_text(item->resolved_token_id,sizeof item->resolved_token_id,token_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;item->winning_scope=scope;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearancePaletteResolutionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5e32d59f2201cccf);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearancePaletteResolution *)0)->role_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearancePaletteResolution *)0)->base_token_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearancePaletteResolution *)0)->resolved_token_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearancePaletteResolutionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearancePaletteResolution *)0)->role_id) - 1U +
        8U + sizeof(((UmiAppearancePaletteResolution *)0)->base_token_id) - 1U +
        8U + sizeof(((UmiAppearancePaletteResolution *)0)->resolved_token_id) - 1U +
        8U;
}
static void UmiAppearancePaletteResolutionArchiveWrite(UmiArchiveWriter *writer, const UmiAppearancePaletteResolution *value)
{
    UmiArchiveWriteText(writer, value->role_id, sizeof(value->role_id));
    UmiArchiveWriteText(writer, value->base_token_id, sizeof(value->base_token_id));
    UmiArchiveWriteText(writer, value->resolved_token_id, sizeof(value->resolved_token_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->winning_scope);
}
static void UmiAppearancePaletteResolutionArchiveRead(UmiArchiveReader *reader, UmiAppearancePaletteResolution *value)
{
    UmiArchiveReadText(reader, value->role_id, sizeof(value->role_id));
    UmiArchiveReadText(reader, value->base_token_id, sizeof(value->base_token_id));
    UmiArchiveReadText(reader, value->resolved_token_id, sizeof(value->resolved_token_id));
    value->winning_scope = (UmiAppearanceScope)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAppearancePaletteResolutionArchiveValidate(const UmiAppearancePaletteResolution *value)
{
    return umi_appearance_palette_resolution_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_palette_resolution_archive_encode, umi_appearance_palette_resolution_archive_decode,
    UmiAppearancePaletteResolution, UmiAppearancePaletteResolutionArchiveSchema, UmiAppearancePaletteResolutionArchiveBound, UmiAppearancePaletteResolutionArchiveWrite, UmiAppearancePaletteResolutionArchiveRead, UmiAppearancePaletteResolutionArchiveValidate)
