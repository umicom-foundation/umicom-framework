/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/contrast_policy.c
 *
 * PURPOSE:
 *   Define certification thresholds for normal text, large text, icons and focus indicators.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/contrast_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_contrast_policy_init(UmiAppearanceContrastPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"contrast.standard");
    item->normal_text_ratio=4.5;
    item->large_text_ratio=3.0;
    item->non_text_ratio=3.0;
    item->focus_ratio=3.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_contrast_policy_is_valid(const UmiAppearanceContrastPolicy *item) {
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
    return (umi_appearance_id_valid(item->policy_id) && item->normal_text_ratio >= 1.0 && item->large_text_ratio >= 1.0 && item->non_text_ratio >= 1.0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceContrastPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8193a14a3a544532);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceContrastPolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceContrastPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceContrastPolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceContrastPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceContrastPolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteDouble(writer, value->normal_text_ratio);
    UmiArchiveWriteDouble(writer, value->large_text_ratio);
    UmiArchiveWriteDouble(writer, value->non_text_ratio);
    UmiArchiveWriteDouble(writer, value->focus_ratio);
}
static void UmiAppearanceContrastPolicyArchiveRead(UmiArchiveReader *reader, UmiAppearanceContrastPolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->normal_text_ratio = UmiArchiveReadDouble(reader);
    value->large_text_ratio = UmiArchiveReadDouble(reader);
    value->non_text_ratio = UmiArchiveReadDouble(reader);
    value->focus_ratio = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAppearanceContrastPolicyArchiveValidate(const UmiAppearanceContrastPolicy *value)
{
    return umi_appearance_contrast_policy_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_contrast_policy_archive_encode, umi_appearance_contrast_policy_archive_decode,
    UmiAppearanceContrastPolicy, UmiAppearanceContrastPolicyArchiveSchema, UmiAppearanceContrastPolicyArchiveBound, UmiAppearanceContrastPolicyArchiveWrite, UmiAppearanceContrastPolicyArchiveRead, UmiAppearanceContrastPolicyArchiveValidate)
