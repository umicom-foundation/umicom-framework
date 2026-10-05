/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/system_appearance_state.c
 *
 * PURPOSE:
 *   Represent operating-system appearance signals without coupling Framework logic to platform APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/system_appearance_state.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_system_appearance_state_init(UmiAppearanceSystemAppearanceState *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->system_id,sizeof item->system_id,"system");
    item->dark_mode=true;
    item->dpi=96U;
    item->scale=1.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_system_appearance_state_is_valid(const UmiAppearanceSystemAppearanceState *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->system_id, '\0', sizeof(item->system_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->system_id) && item->dpi > 0U && item->scale > 0.0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceSystemAppearanceStateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xae569d35f91d927f);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceSystemAppearanceState *)0)->system_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceSystemAppearanceStateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceSystemAppearanceState *)0)->system_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceSystemAppearanceStateArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceSystemAppearanceState *value)
{
    UmiArchiveWriteText(writer, value->system_id, sizeof(value->system_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->dark_mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->high_contrast);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->reduced_motion);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->dpi);
    UmiArchiveWriteDouble(writer, value->scale);
}
static void UmiAppearanceSystemAppearanceStateArchiveRead(UmiArchiveReader *reader, UmiAppearanceSystemAppearanceState *value)
{
    UmiArchiveReadText(reader, value->system_id, sizeof(value->system_id));
    value->dark_mode = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->high_contrast = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->reduced_motion = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->dpi = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->scale = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAppearanceSystemAppearanceStateArchiveValidate(const UmiAppearanceSystemAppearanceState *value)
{
    return umi_appearance_system_appearance_state_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_system_appearance_state_archive_encode, umi_appearance_system_appearance_state_archive_decode,
    UmiAppearanceSystemAppearanceState, UmiAppearanceSystemAppearanceStateArchiveSchema, UmiAppearanceSystemAppearanceStateArchiveBound, UmiAppearanceSystemAppearanceStateArchiveWrite, UmiAppearanceSystemAppearanceStateArchiveRead, UmiAppearanceSystemAppearanceStateArchiveValidate)
