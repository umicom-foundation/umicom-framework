/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/component_state_projection.c
 *
 * PURPOSE:
 *   Map semantic component state to resolved style and accessibility state identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/component_state_projection.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_component_state_projection_init(UmiAppearanceComponentStateProjection *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->component_id,sizeof item->component_id,"button.primary");
    (void)umi_appearance_copy_text(item->state_id,sizeof item->state_id,"normal");
    (void)umi_appearance_copy_text(item->resolved_style_id,sizeof item->resolved_style_id,"style.button.primary.normal");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_component_state_projection_is_valid(const UmiAppearanceComponentStateProjection *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->component_id, '\0', sizeof(item->component_id)) == NULL) return 0;
    if (memchr(item->state_id, '\0', sizeof(item->state_id)) == NULL) return 0;
    if (memchr(item->resolved_style_id, '\0', sizeof(item->resolved_style_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->component_id) && umi_appearance_id_valid(item->state_id) && umi_appearance_id_valid(item->resolved_style_id));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceComponentStateProjectionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7c1e0aa489cacdd8);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceComponentStateProjection *)0)->component_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceComponentStateProjection *)0)->state_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceComponentStateProjection *)0)->resolved_style_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceComponentStateProjectionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceComponentStateProjection *)0)->component_id) - 1U +
        8U + sizeof(((UmiAppearanceComponentStateProjection *)0)->state_id) - 1U +
        8U + sizeof(((UmiAppearanceComponentStateProjection *)0)->resolved_style_id) - 1U +
        8U +
        8U;
}
static void UmiAppearanceComponentStateProjectionArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceComponentStateProjection *value)
{
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
    UmiArchiveWriteText(writer, value->state_id, sizeof(value->state_id));
    UmiArchiveWriteText(writer, value->resolved_style_id, sizeof(value->resolved_style_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->focus_visible);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->disabled);
}
static void UmiAppearanceComponentStateProjectionArchiveRead(UmiArchiveReader *reader, UmiAppearanceComponentStateProjection *value)
{
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
    UmiArchiveReadText(reader, value->state_id, sizeof(value->state_id));
    UmiArchiveReadText(reader, value->resolved_style_id, sizeof(value->resolved_style_id));
    value->focus_visible = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->disabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceComponentStateProjectionArchiveValidate(const UmiAppearanceComponentStateProjection *value)
{
    return umi_appearance_component_state_projection_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_component_state_projection_archive_encode, umi_appearance_component_state_projection_archive_decode,
    UmiAppearanceComponentStateProjection, UmiAppearanceComponentStateProjectionArchiveSchema, UmiAppearanceComponentStateProjectionArchiveBound, UmiAppearanceComponentStateProjectionArchiveWrite, UmiAppearanceComponentStateProjectionArchiveRead, UmiAppearanceComponentStateProjectionArchiveValidate)
