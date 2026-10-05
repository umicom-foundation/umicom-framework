/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/focus_policy.c
 *
 * PURPOSE:
 *   Define visible keyboard-focus treatment requirements across all renderer adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/focus_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_focus_policy_init(UmiAppearanceFocusPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"focus.default");
    item->ring_width=2.0;
    item->ring_offset=2.0;
    item->always_visible_for_keyboard=true;
    item->clip_safe=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_focus_policy_is_valid(const UmiAppearanceFocusPolicy *item) {
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
    return (umi_appearance_id_valid(item->policy_id) && item->ring_width > 0.0 && item->ring_offset >= 0.0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceFocusPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfb4e36475f7acecf);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceFocusPolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceFocusPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceFocusPolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceFocusPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceFocusPolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteDouble(writer, value->ring_width);
    UmiArchiveWriteDouble(writer, value->ring_offset);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->always_visible_for_keyboard);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->clip_safe);
}
static void UmiAppearanceFocusPolicyArchiveRead(UmiArchiveReader *reader, UmiAppearanceFocusPolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->ring_width = UmiArchiveReadDouble(reader);
    value->ring_offset = UmiArchiveReadDouble(reader);
    value->always_visible_for_keyboard = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->clip_safe = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceFocusPolicyArchiveValidate(const UmiAppearanceFocusPolicy *value)
{
    return umi_appearance_focus_policy_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_focus_policy_archive_encode, umi_appearance_focus_policy_archive_decode,
    UmiAppearanceFocusPolicy, UmiAppearanceFocusPolicyArchiveSchema, UmiAppearanceFocusPolicyArchiveBound, UmiAppearanceFocusPolicyArchiveWrite, UmiAppearanceFocusPolicyArchiveRead, UmiAppearanceFocusPolicyArchiveValidate)
