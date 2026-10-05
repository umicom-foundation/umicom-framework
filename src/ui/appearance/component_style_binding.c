/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/component_style_binding.c
 *
 * PURPOSE:
 *   Bind a semantic component and state map to a Framework style identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/component_style_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_component_style_binding_init(UmiAppearanceComponentStyleBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->component_id,sizeof item->component_id,"button.primary");
    (void)umi_appearance_copy_text(item->style_id,sizeof item->style_id,"style.button.primary");
    (void)umi_appearance_copy_text(item->state_map_id,sizeof item->state_map_id,"states.interactive");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_component_style_binding_is_valid(const UmiAppearanceComponentStyleBinding *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->component_id, '\0', sizeof(item->component_id)) == NULL) return 0;
    if (memchr(item->style_id, '\0', sizeof(item->style_id)) == NULL) return 0;
    if (memchr(item->state_map_id, '\0', sizeof(item->state_map_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->component_id) && umi_appearance_id_valid(item->style_id));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceComponentStyleBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe1adfe4a3ce14619);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceComponentStyleBinding *)0)->component_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceComponentStyleBinding *)0)->style_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceComponentStyleBinding *)0)->state_map_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceComponentStyleBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceComponentStyleBinding *)0)->component_id) - 1U +
        8U + sizeof(((UmiAppearanceComponentStyleBinding *)0)->style_id) - 1U +
        8U + sizeof(((UmiAppearanceComponentStyleBinding *)0)->state_map_id) - 1U;
}
static void UmiAppearanceComponentStyleBindingArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceComponentStyleBinding *value)
{
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
    UmiArchiveWriteText(writer, value->style_id, sizeof(value->style_id));
    UmiArchiveWriteText(writer, value->state_map_id, sizeof(value->state_map_id));
}
static void UmiAppearanceComponentStyleBindingArchiveRead(UmiArchiveReader *reader, UmiAppearanceComponentStyleBinding *value)
{
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
    UmiArchiveReadText(reader, value->style_id, sizeof(value->style_id));
    UmiArchiveReadText(reader, value->state_map_id, sizeof(value->state_map_id));
}
static UmiStatus UmiAppearanceComponentStyleBindingArchiveValidate(const UmiAppearanceComponentStyleBinding *value)
{
    return umi_appearance_component_style_binding_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_component_style_binding_archive_encode, umi_appearance_component_style_binding_archive_decode,
    UmiAppearanceComponentStyleBinding, UmiAppearanceComponentStyleBindingArchiveSchema, UmiAppearanceComponentStyleBindingArchiveBound, UmiAppearanceComponentStyleBindingArchiveWrite, UmiAppearanceComponentStyleBindingArchiveRead, UmiAppearanceComponentStyleBindingArchiveValidate)
