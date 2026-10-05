/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/motion_policy.c
 *
 * PURPOSE:
 *   Define semantic motion allowances and maximum transition durations for production UI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/motion_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_motion_policy_init(UmiAppearanceMotionPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"motion.default");
    item->standard_duration_ms=150U;
    item->emphasis_duration_ms=250U;
    item->allow_decorative_motion=true;
    item->allow_parallax=false;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_motion_policy_is_valid(const UmiAppearanceMotionPolicy *item) {
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
    return (umi_appearance_id_valid(item->policy_id) && item->standard_duration_ms <= 2000U && item->emphasis_duration_ms <= 3000U);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceMotionPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf552fd1d90846925);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceMotionPolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceMotionPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceMotionPolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceMotionPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceMotionPolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->standard_duration_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->emphasis_duration_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_decorative_motion);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_parallax);
}
static void UmiAppearanceMotionPolicyArchiveRead(UmiArchiveReader *reader, UmiAppearanceMotionPolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->standard_duration_ms = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->emphasis_duration_ms = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->allow_decorative_motion = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->allow_parallax = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceMotionPolicyArchiveValidate(const UmiAppearanceMotionPolicy *value)
{
    return umi_appearance_motion_policy_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_motion_policy_archive_encode, umi_appearance_motion_policy_archive_decode,
    UmiAppearanceMotionPolicy, UmiAppearanceMotionPolicyArchiveSchema, UmiAppearanceMotionPolicyArchiveBound, UmiAppearanceMotionPolicyArchiveWrite, UmiAppearanceMotionPolicyArchiveRead, UmiAppearanceMotionPolicyArchiveValidate)
