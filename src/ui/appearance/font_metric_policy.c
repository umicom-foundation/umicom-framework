/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/font_metric_policy.c
 *
 * PURPOSE:
 *   Define renderer-neutral font metric tolerances used to prevent clipping and layout drift.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_metric_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_font_metric_policy_init(UmiAppearanceFontMetricPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"metrics.ui");
    item->minimum_x_height_ratio=0.45;
    item->maximum_line_gap_ratio=0.50;
    item->baseline_tolerance_dp=1.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_font_metric_policy_is_valid(const UmiAppearanceFontMetricPolicy *item) {
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
    return (umi_appearance_id_valid(item->policy_id) && item->minimum_x_height_ratio > 0.0 && item->maximum_line_gap_ratio >= 0.0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceFontMetricPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xbe6092d455840f27);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceFontMetricPolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceFontMetricPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceFontMetricPolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceFontMetricPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceFontMetricPolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteDouble(writer, value->minimum_x_height_ratio);
    UmiArchiveWriteDouble(writer, value->maximum_line_gap_ratio);
    UmiArchiveWriteDouble(writer, value->baseline_tolerance_dp);
}
static void UmiAppearanceFontMetricPolicyArchiveRead(UmiArchiveReader *reader, UmiAppearanceFontMetricPolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->minimum_x_height_ratio = UmiArchiveReadDouble(reader);
    value->maximum_line_gap_ratio = UmiArchiveReadDouble(reader);
    value->baseline_tolerance_dp = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAppearanceFontMetricPolicyArchiveValidate(const UmiAppearanceFontMetricPolicy *value)
{
    return umi_appearance_font_metric_policy_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_font_metric_policy_archive_encode, umi_appearance_font_metric_policy_archive_decode,
    UmiAppearanceFontMetricPolicy, UmiAppearanceFontMetricPolicyArchiveSchema, UmiAppearanceFontMetricPolicyArchiveBound, UmiAppearanceFontMetricPolicyArchiveWrite, UmiAppearanceFontMetricPolicyArchiveRead, UmiAppearanceFontMetricPolicyArchiveValidate)
