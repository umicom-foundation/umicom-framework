/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/input_target_policy.c
 *
 * PURPOSE:
 *   Resolve minimum interactive target dimensions by pointer, touch, keyboard or hybrid modality.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/input_target_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_input_target_policy_init(UmiAppearanceInputTargetPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"target.pointer");
    item->modality=UMI_APPEARANCE_INPUT_POINTER;
    item->minimum_width_dp=24.0;
    item->minimum_height_dp=24.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_input_target_policy_is_valid(const UmiAppearanceInputTargetPolicy *item) {
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
    return (umi_appearance_id_valid(item->policy_id) && item->minimum_width_dp > 0.0 && item->minimum_height_dp > 0.0);
}
/*
 * Provide the appearance input target policy for modality operation used by this module
 * and its client applications.
 */
UmiStatus umi_appearance_input_target_policy_for_modality(UmiAppearanceInputTargetPolicy *item,UmiAppearanceInputModality modality){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;item->modality=modality;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(modality==UMI_APPEARANCE_INPUT_TOUCH||modality==UMI_APPEARANCE_INPUT_HYBRID){item->minimum_width_dp=44.0;item->minimum_height_dp=44.0;}/* Use this fallback path when the earlier condition does not apply. */ else{item->minimum_width_dp=24.0;item->minimum_height_dp=24.0;}return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceInputTargetPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xbea72b86aa10ec46);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceInputTargetPolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceInputTargetPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceInputTargetPolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceInputTargetPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceInputTargetPolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->modality);
    UmiArchiveWriteDouble(writer, value->minimum_width_dp);
    UmiArchiveWriteDouble(writer, value->minimum_height_dp);
}
static void UmiAppearanceInputTargetPolicyArchiveRead(UmiArchiveReader *reader, UmiAppearanceInputTargetPolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->modality = (UmiAppearanceInputModality)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->minimum_width_dp = UmiArchiveReadDouble(reader);
    value->minimum_height_dp = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAppearanceInputTargetPolicyArchiveValidate(const UmiAppearanceInputTargetPolicy *value)
{
    return umi_appearance_input_target_policy_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_input_target_policy_archive_encode, umi_appearance_input_target_policy_archive_decode,
    UmiAppearanceInputTargetPolicy, UmiAppearanceInputTargetPolicyArchiveSchema, UmiAppearanceInputTargetPolicyArchiveBound, UmiAppearanceInputTargetPolicyArchiveWrite, UmiAppearanceInputTargetPolicyArchiveRead, UmiAppearanceInputTargetPolicyArchiveValidate)
