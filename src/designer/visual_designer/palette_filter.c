/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/palette_filter.c
 *
 * PURPOSE:
 *   Filter the component palette by text, category and capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/palette_filter.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer palette filter from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_palette_filter_init(UmiRadPaletteFilter *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->query, sizeof item->query, "palette_filter");
    (void)umi_rad_copy_text(item->category, sizeof item->category, "palette_filter");
    (void)umi_rad_copy_text(item->capability, sizeof item->capability, "palette_filter");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer palette filter satisfies its contract before another service relies on
 * it.
 */
int umi_rad_palette_filter_is_valid(const UmiRadPaletteFilter *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->query, '\0', sizeof(item->query)) == NULL) return 0;
    if (memchr(item->category, '\0', sizeof(item->category)) == NULL) return 0;
    if (memchr(item->capability, '\0', sizeof(item->capability)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return 1;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadPaletteFilterArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4296f11091632124);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPaletteFilter *)0)->query)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPaletteFilter *)0)->category)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPaletteFilter *)0)->capability)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadPaletteFilterArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadPaletteFilter *)0)->query) - 1U +
        8U + sizeof(((UmiRadPaletteFilter *)0)->category) - 1U +
        8U + sizeof(((UmiRadPaletteFilter *)0)->capability) - 1U +
        8U;
}
static void UmiRadPaletteFilterArchiveWrite(UmiArchiveWriter *writer, const UmiRadPaletteFilter *value)
{
    UmiArchiveWriteText(writer, value->query, sizeof(value->query));
    UmiArchiveWriteText(writer, value->category, sizeof(value->category));
    UmiArchiveWriteText(writer, value->capability, sizeof(value->capability));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->favourites_only);
}
static void UmiRadPaletteFilterArchiveRead(UmiArchiveReader *reader, UmiRadPaletteFilter *value)
{
    UmiArchiveReadText(reader, value->query, sizeof(value->query));
    UmiArchiveReadText(reader, value->category, sizeof(value->category));
    UmiArchiveReadText(reader, value->capability, sizeof(value->capability));
    value->favourites_only = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadPaletteFilterArchiveValidate(const UmiRadPaletteFilter *value)
{
    return umi_rad_palette_filter_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_palette_filter_archive_encode, umi_rad_palette_filter_archive_decode,
    UmiRadPaletteFilter, UmiRadPaletteFilterArchiveSchema, UmiRadPaletteFilterArchiveBound, UmiRadPaletteFilterArchiveWrite, UmiRadPaletteFilterArchiveRead, UmiRadPaletteFilterArchiveValidate)
