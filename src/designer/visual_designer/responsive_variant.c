/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/responsive_variant.c
 *
 * PURPOSE:
 *   Describe per-breakpoint component geometry and visibility overrides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/responsive_variant.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer responsive variant from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_responsive_variant_init(UmiRadResponsiveVariant *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->breakpoint_id, sizeof item->breakpoint_id, "responsive_variant");
    item->visible = true;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer responsive variant satisfies its contract before another service relies
 * on it.
 */
int umi_rad_responsive_variant_is_valid(const UmiRadResponsiveVariant *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->breakpoint_id, '\0', sizeof(item->breakpoint_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->breakpoint_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadResponsiveVariantArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x823ef786e2ce47fb);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadResponsiveVariant *)0)->breakpoint_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadResponsiveVariantArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadResponsiveVariant *)0)->breakpoint_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadResponsiveVariantArchiveWrite(UmiArchiveWriter *writer, const UmiRadResponsiveVariant *value)
{
    UmiArchiveWriteText(writer, value->breakpoint_id, sizeof(value->breakpoint_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.x);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.y);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.width);
    UmiArchiveWriteSigned(writer, (int64_t)value->bounds.height);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->override_geometry);
}
static void UmiRadResponsiveVariantArchiveRead(UmiArchiveReader *reader, UmiRadResponsiveVariant *value)
{
    UmiArchiveReadText(reader, value->breakpoint_id, sizeof(value->breakpoint_id));
    value->bounds.x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->bounds.height = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->visible = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->override_geometry = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadResponsiveVariantArchiveValidate(const UmiRadResponsiveVariant *value)
{
    return umi_rad_responsive_variant_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_responsive_variant_archive_encode, umi_rad_responsive_variant_archive_decode,
    UmiRadResponsiveVariant, UmiRadResponsiveVariantArchiveSchema, UmiRadResponsiveVariantArchiveBound, UmiRadResponsiveVariantArchiveWrite, UmiRadResponsiveVariantArchiveRead, UmiRadResponsiveVariantArchiveValidate)
