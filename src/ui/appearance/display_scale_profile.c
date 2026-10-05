/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/display_scale_profile.c
 *
 * PURPOSE:
 *   Combine display DPI, operating-system scale and user accessibility scale into one profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/display_scale_profile.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_display_scale_profile_init(UmiAppearanceDisplayScaleProfile *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->display_id,sizeof item->display_id,"display.primary");
    item->dpi=96U;
    item->os_scale=1.0;
    item->user_scale=1.0;
    item->effective_scale=1.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_display_scale_profile_is_valid(const UmiAppearanceDisplayScaleProfile *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->display_id, '\0', sizeof(item->display_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->display_id) && item->dpi > 0U && item->os_scale > 0.0 && item->user_scale > 0.0 && item->effective_scale > 0.0);
}
/*
 * Provide the appearance display scale profile resolve operation used by this module and
 * its client applications.
 */
UmiStatus umi_appearance_display_scale_profile_resolve(UmiAppearanceDisplayScaleProfile *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||item->os_scale<=0.0||item->user_scale<=0.0)return UMI_STATUS_INVALID_ARGUMENT;item->effective_scale=item->os_scale*item->user_scale;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceDisplayScaleProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdce219659c21be87);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceDisplayScaleProfile *)0)->display_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceDisplayScaleProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceDisplayScaleProfile *)0)->display_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceDisplayScaleProfileArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceDisplayScaleProfile *value)
{
    UmiArchiveWriteText(writer, value->display_id, sizeof(value->display_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->dpi);
    UmiArchiveWriteDouble(writer, value->os_scale);
    UmiArchiveWriteDouble(writer, value->user_scale);
    UmiArchiveWriteDouble(writer, value->effective_scale);
}
static void UmiAppearanceDisplayScaleProfileArchiveRead(UmiArchiveReader *reader, UmiAppearanceDisplayScaleProfile *value)
{
    UmiArchiveReadText(reader, value->display_id, sizeof(value->display_id));
    value->dpi = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->os_scale = UmiArchiveReadDouble(reader);
    value->user_scale = UmiArchiveReadDouble(reader);
    value->effective_scale = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAppearanceDisplayScaleProfileArchiveValidate(const UmiAppearanceDisplayScaleProfile *value)
{
    return umi_appearance_display_scale_profile_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_display_scale_profile_archive_encode, umi_appearance_display_scale_profile_archive_decode,
    UmiAppearanceDisplayScaleProfile, UmiAppearanceDisplayScaleProfileArchiveSchema, UmiAppearanceDisplayScaleProfileArchiveBound, UmiAppearanceDisplayScaleProfileArchiveWrite, UmiAppearanceDisplayScaleProfileArchiveRead, UmiAppearanceDisplayScaleProfileArchiveValidate)
