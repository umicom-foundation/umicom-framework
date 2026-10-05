/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/icon_scale_policy.c
 *
 * PURPOSE:
 *   Resolve logical icon size to physical pixels using the effective display scale.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_scale_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_icon_scale_policy_init(UmiAppearanceIconScalePolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"icon.default");
    item->logical_size_dp=16.0;
    item->scale=1.0;
    item->physical_size_px=16U;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_icon_scale_policy_is_valid(const UmiAppearanceIconScalePolicy *item) {
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
    return (umi_appearance_id_valid(item->policy_id) && item->logical_size_dp > 0.0 && item->scale > 0.0 && item->physical_size_px > 0U);
}
#include <math.h>
/*
 * Provide the appearance icon scale policy resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_appearance_icon_scale_policy_resolve(UmiAppearanceIconScalePolicy *item){double pixels;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||item->logical_size_dp<=0.0||item->scale<=0.0)return UMI_STATUS_INVALID_ARGUMENT;pixels=item->logical_size_dp*item->scale;item->physical_size_px=(uint32_t)floor(pixels+0.5);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item->physical_size_px==0U)item->physical_size_px=1U;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceIconScalePolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc71ba68c5a9effba);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceIconScalePolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceIconScalePolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceIconScalePolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceIconScalePolicyArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceIconScalePolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteDouble(writer, value->logical_size_dp);
    UmiArchiveWriteDouble(writer, value->scale);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->physical_size_px);
}
static void UmiAppearanceIconScalePolicyArchiveRead(UmiArchiveReader *reader, UmiAppearanceIconScalePolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->logical_size_dp = UmiArchiveReadDouble(reader);
    value->scale = UmiArchiveReadDouble(reader);
    value->physical_size_px = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiAppearanceIconScalePolicyArchiveValidate(const UmiAppearanceIconScalePolicy *value)
{
    return umi_appearance_icon_scale_policy_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_icon_scale_policy_archive_encode, umi_appearance_icon_scale_policy_archive_decode,
    UmiAppearanceIconScalePolicy, UmiAppearanceIconScalePolicyArchiveSchema, UmiAppearanceIconScalePolicyArchiveBound, UmiAppearanceIconScalePolicyArchiveWrite, UmiAppearanceIconScalePolicyArchiveRead, UmiAppearanceIconScalePolicyArchiveValidate)
