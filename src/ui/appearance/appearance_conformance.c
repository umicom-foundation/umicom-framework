/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/appearance_conformance.c
 *
 * PURPOSE:
 *   Define release-gate requirements for theme, high-DPI, accessibility and frontend appearance parity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_conformance.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_conformance_init(UmiAppearanceAppearanceConformance *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->profile_id,sizeof item->profile_id,"appearance.release");
    item->require_theme_parity=true;
    item->require_high_dpi=true;
    item->require_contrast=true;
    item->require_keyboard_focus=true;
    item->require_reduced_motion=true;
    item->passed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_conformance_is_valid(const UmiAppearanceAppearanceConformance *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->profile_id, '\0', sizeof(item->profile_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->profile_id));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceAppearanceConformanceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdb4df03a1ed57305);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceAppearanceConformance *)0)->profile_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceAppearanceConformanceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceAppearanceConformance *)0)->profile_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceAppearanceConformanceArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceAppearanceConformance *value)
{
    UmiArchiveWriteText(writer, value->profile_id, sizeof(value->profile_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_theme_parity);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_high_dpi);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_contrast);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_keyboard_focus);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_reduced_motion);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->passed);
}
static void UmiAppearanceAppearanceConformanceArchiveRead(UmiArchiveReader *reader, UmiAppearanceAppearanceConformance *value)
{
    UmiArchiveReadText(reader, value->profile_id, sizeof(value->profile_id));
    value->require_theme_parity = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->require_high_dpi = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->require_contrast = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->require_keyboard_focus = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->require_reduced_motion = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->passed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceAppearanceConformanceArchiveValidate(const UmiAppearanceAppearanceConformance *value)
{
    return umi_appearance_conformance_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_conformance_archive_encode, umi_appearance_conformance_archive_decode,
    UmiAppearanceAppearanceConformance, UmiAppearanceAppearanceConformanceArchiveSchema, UmiAppearanceAppearanceConformanceArchiveBound, UmiAppearanceAppearanceConformanceArchiveWrite, UmiAppearanceAppearanceConformanceArchiveRead, UmiAppearanceAppearanceConformanceArchiveValidate)
