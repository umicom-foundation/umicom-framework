/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/renderer_appearance_capability.c
 *
 * PURPOSE:
 *   Declare appearance capabilities and limitations for GTK4, Qt6, Native Web or headless renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/renderer_appearance_capability.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_renderer_appearance_capability_init(UmiAppearanceRendererAppearanceCapability *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->renderer_id,sizeof item->renderer_id,"gtk4");
    item->kind=UMI_APPEARANCE_RENDERER_GTK4;
    item->supports_fractional_scale=true;
    item->supports_high_contrast=true;
    item->supports_reduced_motion=true;
    item->supports_symbolic_icons=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_renderer_appearance_capability_is_valid(const UmiAppearanceRendererAppearanceCapability *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->renderer_id, '\0', sizeof(item->renderer_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->renderer_id) && item->kind >= UMI_APPEARANCE_RENDERER_GTK4 && item->kind <= UMI_APPEARANCE_RENDERER_HEADLESS);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceRendererAppearanceCapabilityArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3394d1b806f55b1c);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceRendererAppearanceCapability *)0)->renderer_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceRendererAppearanceCapabilityArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceRendererAppearanceCapability *)0)->renderer_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceRendererAppearanceCapabilityArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceRendererAppearanceCapability *value)
{
    UmiArchiveWriteText(writer, value->renderer_id, sizeof(value->renderer_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->supports_fractional_scale);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->supports_high_contrast);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->supports_reduced_motion);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->supports_symbolic_icons);
}
static void UmiAppearanceRendererAppearanceCapabilityArchiveRead(UmiArchiveReader *reader, UmiAppearanceRendererAppearanceCapability *value)
{
    UmiArchiveReadText(reader, value->renderer_id, sizeof(value->renderer_id));
    value->kind = (UmiAppearanceRendererKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->supports_fractional_scale = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->supports_high_contrast = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->supports_reduced_motion = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->supports_symbolic_icons = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceRendererAppearanceCapabilityArchiveValidate(const UmiAppearanceRendererAppearanceCapability *value)
{
    return umi_appearance_renderer_appearance_capability_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_renderer_appearance_capability_archive_encode, umi_appearance_renderer_appearance_capability_archive_decode,
    UmiAppearanceRendererAppearanceCapability, UmiAppearanceRendererAppearanceCapabilityArchiveSchema, UmiAppearanceRendererAppearanceCapabilityArchiveBound, UmiAppearanceRendererAppearanceCapabilityArchiveWrite, UmiAppearanceRendererAppearanceCapabilityArchiveRead, UmiAppearanceRendererAppearanceCapabilityArchiveValidate)
