/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/line_height_policy.c
 *
 * PURPOSE:
 *   Maintain readable line-height bounds as font and accessibility scale changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/line_height_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_line_height_policy_init(UmiAppearanceLineHeightPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"line-height.default");
    item->minimum_multiplier=1.1;
    item->preferred_multiplier=1.4;
    item->maximum_multiplier=2.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_line_height_policy_is_valid(const UmiAppearanceLineHeightPolicy *item) {
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
    return (umi_appearance_id_valid(item->policy_id) && item->minimum_multiplier > 0.0 && item->preferred_multiplier >= item->minimum_multiplier && item->maximum_multiplier >= item->preferred_multiplier);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceLineHeightPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0501c3b744595d9f);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceLineHeightPolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceLineHeightPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceLineHeightPolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceLineHeightPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceLineHeightPolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteDouble(writer, value->minimum_multiplier);
    UmiArchiveWriteDouble(writer, value->preferred_multiplier);
    UmiArchiveWriteDouble(writer, value->maximum_multiplier);
}
static void UmiAppearanceLineHeightPolicyArchiveRead(UmiArchiveReader *reader, UmiAppearanceLineHeightPolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->minimum_multiplier = UmiArchiveReadDouble(reader);
    value->preferred_multiplier = UmiArchiveReadDouble(reader);
    value->maximum_multiplier = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAppearanceLineHeightPolicyArchiveValidate(const UmiAppearanceLineHeightPolicy *value)
{
    return umi_appearance_line_height_policy_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_line_height_policy_archive_encode, umi_appearance_line_height_policy_archive_decode,
    UmiAppearanceLineHeightPolicy, UmiAppearanceLineHeightPolicyArchiveSchema, UmiAppearanceLineHeightPolicyArchiveBound, UmiAppearanceLineHeightPolicyArchiveWrite, UmiAppearanceLineHeightPolicyArchiveRead, UmiAppearanceLineHeightPolicyArchiveValidate)
