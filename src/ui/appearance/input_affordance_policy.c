/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/input_affordance_policy.c
 *
 * PURPOSE:
 *   Require hover, focus, pressed and touch feedback appropriate to available input modalities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/input_affordance_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_input_affordance_policy_init(UmiAppearanceInputAffordancePolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"affordance.hybrid");
    item->require_hover_feedback=true;
    item->require_focus_feedback=true;
    item->require_pressed_feedback=true;
    item->require_touch_feedback=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_input_affordance_policy_is_valid(const UmiAppearanceInputAffordancePolicy *item) {
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
    return (umi_appearance_id_valid(item->policy_id) && item->require_focus_feedback && item->require_pressed_feedback);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceInputAffordancePolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf64e3805ea3911f3);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceInputAffordancePolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceInputAffordancePolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceInputAffordancePolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceInputAffordancePolicyArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceInputAffordancePolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_hover_feedback);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_focus_feedback);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_pressed_feedback);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_touch_feedback);
}
static void UmiAppearanceInputAffordancePolicyArchiveRead(UmiArchiveReader *reader, UmiAppearanceInputAffordancePolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->require_hover_feedback = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->require_focus_feedback = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->require_pressed_feedback = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->require_touch_feedback = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceInputAffordancePolicyArchiveValidate(const UmiAppearanceInputAffordancePolicy *value)
{
    return umi_appearance_input_affordance_policy_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_input_affordance_policy_archive_encode, umi_appearance_input_affordance_policy_archive_decode,
    UmiAppearanceInputAffordancePolicy, UmiAppearanceInputAffordancePolicyArchiveSchema, UmiAppearanceInputAffordancePolicyArchiveBound, UmiAppearanceInputAffordancePolicyArchiveWrite, UmiAppearanceInputAffordancePolicyArchiveRead, UmiAppearanceInputAffordancePolicyArchiveValidate)
