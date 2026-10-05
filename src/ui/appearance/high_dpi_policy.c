/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/high_dpi_policy.c
 *
 * PURPOSE:
 *   Define fractional-layout, snapping and asset-resolution rules for high-DPI displays.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/high_dpi_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_high_dpi_policy_init(UmiAppearanceHighDpiPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"dpi.production");
    item->allow_fractional_layout=true;
    item->snap_hairlines=true;
    item->prefer_vector_icons=true;
    item->maximum_raster_scale=4U;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_high_dpi_policy_is_valid(const UmiAppearanceHighDpiPolicy *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->policy_id, '\0', sizeof(item->policy_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->maximum_raster_scale >= 1U);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceHighDpiPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa6110c0df32e8f41);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceHighDpiPolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceHighDpiPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceHighDpiPolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceHighDpiPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceHighDpiPolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_fractional_layout);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->snap_hairlines);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->prefer_vector_icons);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_raster_scale);
}
static void UmiAppearanceHighDpiPolicyArchiveRead(UmiArchiveReader *reader, UmiAppearanceHighDpiPolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->allow_fractional_layout = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->snap_hairlines = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->prefer_vector_icons = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->maximum_raster_scale = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiAppearanceHighDpiPolicyArchiveValidate(const UmiAppearanceHighDpiPolicy *value)
{
    return umi_appearance_high_dpi_policy_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_high_dpi_policy_archive_encode, umi_appearance_high_dpi_policy_archive_decode,
    UmiAppearanceHighDpiPolicy, UmiAppearanceHighDpiPolicyArchiveSchema, UmiAppearanceHighDpiPolicyArchiveBound, UmiAppearanceHighDpiPolicyArchiveWrite, UmiAppearanceHighDpiPolicyArchiveRead, UmiAppearanceHighDpiPolicyArchiveValidate)
