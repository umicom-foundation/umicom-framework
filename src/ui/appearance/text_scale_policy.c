/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/text_scale_policy.c
 *
 * PURPOSE:
 *   Clamp user text scaling while preserving semantic size hierarchy and accessibility intent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/text_scale_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_text_scale_policy_init(UmiAppearanceTextScalePolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"text-scale.default");
    item->minimum_scale=0.8;
    item->maximum_scale=3.0;
    item->requested_scale=1.0;
    item->resolved_scale=1.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_text_scale_policy_is_valid(const UmiAppearanceTextScalePolicy *item) {
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
    return (umi_appearance_id_valid(item->policy_id) && item->minimum_scale > 0.0 && item->maximum_scale >= item->minimum_scale);
}
/*
 * Provide the appearance text scale policy resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_appearance_text_scale_policy_resolve(UmiAppearanceTextScalePolicy *item,double requested){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;item->requested_scale=requested;return umi_appearance_clamp_scale(requested,item->minimum_scale,item->maximum_scale,&item->resolved_scale);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceTextScalePolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfd24a55de7f50bdc);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceTextScalePolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceTextScalePolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceTextScalePolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceTextScalePolicyArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceTextScalePolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteDouble(writer, value->minimum_scale);
    UmiArchiveWriteDouble(writer, value->maximum_scale);
    UmiArchiveWriteDouble(writer, value->requested_scale);
    UmiArchiveWriteDouble(writer, value->resolved_scale);
}
static void UmiAppearanceTextScalePolicyArchiveRead(UmiArchiveReader *reader, UmiAppearanceTextScalePolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->minimum_scale = UmiArchiveReadDouble(reader);
    value->maximum_scale = UmiArchiveReadDouble(reader);
    value->requested_scale = UmiArchiveReadDouble(reader);
    value->resolved_scale = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAppearanceTextScalePolicyArchiveValidate(const UmiAppearanceTextScalePolicy *value)
{
    return umi_appearance_text_scale_policy_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_text_scale_policy_archive_encode, umi_appearance_text_scale_policy_archive_decode,
    UmiAppearanceTextScalePolicy, UmiAppearanceTextScalePolicyArchiveSchema, UmiAppearanceTextScalePolicyArchiveBound, UmiAppearanceTextScalePolicyArchiveWrite, UmiAppearanceTextScalePolicyArchiveRead, UmiAppearanceTextScalePolicyArchiveValidate)
