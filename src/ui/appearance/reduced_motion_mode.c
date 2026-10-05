/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/reduced_motion_mode.c
 *
 * PURPOSE:
 *   Resolve reduced-motion presentation requirements from user and system accessibility settings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/reduced_motion_mode.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_reduced_motion_mode_init(UmiAppearanceReducedMotionMode *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->mode_id,sizeof item->mode_id,"motion.reduced");
    item->enabled=true;
    item->maximum_duration_ms=80U;
    item->disable_decorative=true;
    item->preserve_essential_feedback=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_reduced_motion_mode_is_valid(const UmiAppearanceReducedMotionMode *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->mode_id, '\0', sizeof(item->mode_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->mode_id) && item->maximum_duration_ms <= 500U);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceReducedMotionModeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x9de24cf49b9d4895);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceReducedMotionMode *)0)->mode_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceReducedMotionModeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceReducedMotionMode *)0)->mode_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceReducedMotionModeArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceReducedMotionMode *value)
{
    UmiArchiveWriteText(writer, value->mode_id, sizeof(value->mode_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_duration_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->disable_decorative);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->preserve_essential_feedback);
}
static void UmiAppearanceReducedMotionModeArchiveRead(UmiArchiveReader *reader, UmiAppearanceReducedMotionMode *value)
{
    UmiArchiveReadText(reader, value->mode_id, sizeof(value->mode_id));
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->maximum_duration_ms = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->disable_decorative = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->preserve_essential_feedback = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceReducedMotionModeArchiveValidate(const UmiAppearanceReducedMotionMode *value)
{
    return umi_appearance_reduced_motion_mode_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_reduced_motion_mode_archive_encode, umi_appearance_reduced_motion_mode_archive_decode,
    UmiAppearanceReducedMotionMode, UmiAppearanceReducedMotionModeArchiveSchema, UmiAppearanceReducedMotionModeArchiveBound, UmiAppearanceReducedMotionModeArchiveWrite, UmiAppearanceReducedMotionModeArchiveRead, UmiAppearanceReducedMotionModeArchiveValidate)
