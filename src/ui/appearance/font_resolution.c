/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/font_resolution.c
 *
 * PURPOSE:
 *   Record the winning family and fallback depth selected for a semantic font stack.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_resolution.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_font_resolution_init(UmiAppearanceFontResolution *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->stack_id,sizeof item->stack_id,"ui");
    (void)umi_appearance_copy_text(item->resolved_family_id,sizeof item->resolved_family_id,"font.primary");
    item->available=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_font_resolution_is_valid(const UmiAppearanceFontResolution *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->stack_id, '\0', sizeof(item->stack_id)) == NULL) return 0;
    if (memchr(item->resolved_family_id, '\0', sizeof(item->resolved_family_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->stack_id) && (!item->available || umi_appearance_id_valid(item->resolved_family_id)));
}
/*
 * Provide the appearance font resolution choose operation used by this module and its
 * client applications.
 */
UmiStatus umi_appearance_font_resolution_choose(UmiAppearanceFontResolution *item,const char *preferred,const char *fallback){const char *chosen; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; chosen=(preferred&&preferred[0])?preferred:fallback; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_appearance_id_valid(chosen)){item->available=false;item->resolved_family_id[0]=0;return UMI_STATUS_NOT_FOUND;} item->available=true;item->fallback_depth=(preferred&&preferred[0])?0U:1U;return umi_appearance_copy_text(item->resolved_family_id,sizeof item->resolved_family_id,chosen);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceFontResolutionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x05eaf776b2304e1f);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceFontResolution *)0)->stack_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceFontResolution *)0)->resolved_family_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceFontResolutionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceFontResolution *)0)->stack_id) - 1U +
        8U + sizeof(((UmiAppearanceFontResolution *)0)->resolved_family_id) - 1U +
        8U +
        8U;
}
static void UmiAppearanceFontResolutionArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceFontResolution *value)
{
    UmiArchiveWriteText(writer, value->stack_id, sizeof(value->stack_id));
    UmiArchiveWriteText(writer, value->resolved_family_id, sizeof(value->resolved_family_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->fallback_depth);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->available);
}
static void UmiAppearanceFontResolutionArchiveRead(UmiArchiveReader *reader, UmiAppearanceFontResolution *value)
{
    UmiArchiveReadText(reader, value->stack_id, sizeof(value->stack_id));
    UmiArchiveReadText(reader, value->resolved_family_id, sizeof(value->resolved_family_id));
    value->fallback_depth = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->available = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceFontResolutionArchiveValidate(const UmiAppearanceFontResolution *value)
{
    return umi_appearance_font_resolution_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_font_resolution_archive_encode, umi_appearance_font_resolution_archive_decode,
    UmiAppearanceFontResolution, UmiAppearanceFontResolutionArchiveSchema, UmiAppearanceFontResolutionArchiveBound, UmiAppearanceFontResolutionArchiveWrite, UmiAppearanceFontResolutionArchiveRead, UmiAppearanceFontResolutionArchiveValidate)
