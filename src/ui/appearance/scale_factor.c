/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/scale_factor.c
 *
 * PURPOSE:
 *   Represent a bounded effective UI scale factor with independent OS and user contributions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/scale_factor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_scale_factor_init(UmiAppearanceScaleFactor *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->scale_id,sizeof item->scale_id,"scale.default");
    item->os_factor=1.0;
    item->user_factor=1.0;
    item->effective_factor=1.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_scale_factor_is_valid(const UmiAppearanceScaleFactor *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->scale_id, '\0', sizeof(item->scale_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->scale_id) && item->os_factor > 0.0 && item->user_factor > 0.0 && item->effective_factor > 0.0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceScaleFactorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1ff1ae8c35b8ed78);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceScaleFactor *)0)->scale_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceScaleFactorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceScaleFactor *)0)->scale_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceScaleFactorArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceScaleFactor *value)
{
    UmiArchiveWriteText(writer, value->scale_id, sizeof(value->scale_id));
    UmiArchiveWriteDouble(writer, value->os_factor);
    UmiArchiveWriteDouble(writer, value->user_factor);
    UmiArchiveWriteDouble(writer, value->effective_factor);
}
static void UmiAppearanceScaleFactorArchiveRead(UmiArchiveReader *reader, UmiAppearanceScaleFactor *value)
{
    UmiArchiveReadText(reader, value->scale_id, sizeof(value->scale_id));
    value->os_factor = UmiArchiveReadDouble(reader);
    value->user_factor = UmiArchiveReadDouble(reader);
    value->effective_factor = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAppearanceScaleFactorArchiveValidate(const UmiAppearanceScaleFactor *value)
{
    return umi_appearance_scale_factor_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_scale_factor_archive_encode, umi_appearance_scale_factor_archive_decode,
    UmiAppearanceScaleFactor, UmiAppearanceScaleFactorArchiveSchema, UmiAppearanceScaleFactorArchiveBound, UmiAppearanceScaleFactorArchiveWrite, UmiAppearanceScaleFactorArchiveRead, UmiAppearanceScaleFactorArchiveValidate)
