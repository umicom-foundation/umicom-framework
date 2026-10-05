/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/shortcut_presentation.c
 *
 * PURPOSE:
 *   Describe platform-neutral command shortcut hints for menus, toolbars and palettes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/shortcut_presentation.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_shortcut_presentation_init(UmiAppearanceShortcutPresentation *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->action_id,sizeof item->action_id,"file.save");
    (void)umi_appearance_copy_text(item->accelerator_id,sizeof item->accelerator_id,"primary+s");
    (void)umi_appearance_copy_text(item->display_text,sizeof item->display_text,"Save");
    item->discoverable=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_shortcut_presentation_is_valid(const UmiAppearanceShortcutPresentation *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->action_id, '\0', sizeof(item->action_id)) == NULL) return 0;
    if (memchr(item->accelerator_id, '\0', sizeof(item->accelerator_id)) == NULL) return 0;
    if (memchr(item->display_text, '\0', sizeof(item->display_text)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->action_id) && umi_appearance_id_valid(item->accelerator_id));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceShortcutPresentationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc5d9ac10cda1f2b2);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceShortcutPresentation *)0)->action_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceShortcutPresentation *)0)->accelerator_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceShortcutPresentation *)0)->display_text)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceShortcutPresentationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceShortcutPresentation *)0)->action_id) - 1U +
        8U + sizeof(((UmiAppearanceShortcutPresentation *)0)->accelerator_id) - 1U +
        8U + sizeof(((UmiAppearanceShortcutPresentation *)0)->display_text) - 1U +
        8U;
}
static void UmiAppearanceShortcutPresentationArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceShortcutPresentation *value)
{
    UmiArchiveWriteText(writer, value->action_id, sizeof(value->action_id));
    UmiArchiveWriteText(writer, value->accelerator_id, sizeof(value->accelerator_id));
    UmiArchiveWriteText(writer, value->display_text, sizeof(value->display_text));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->discoverable);
}
static void UmiAppearanceShortcutPresentationArchiveRead(UmiArchiveReader *reader, UmiAppearanceShortcutPresentation *value)
{
    UmiArchiveReadText(reader, value->action_id, sizeof(value->action_id));
    UmiArchiveReadText(reader, value->accelerator_id, sizeof(value->accelerator_id));
    UmiArchiveReadText(reader, value->display_text, sizeof(value->display_text));
    value->discoverable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceShortcutPresentationArchiveValidate(const UmiAppearanceShortcutPresentation *value)
{
    return umi_appearance_shortcut_presentation_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_shortcut_presentation_archive_encode, umi_appearance_shortcut_presentation_archive_decode,
    UmiAppearanceShortcutPresentation, UmiAppearanceShortcutPresentationArchiveSchema, UmiAppearanceShortcutPresentationArchiveBound, UmiAppearanceShortcutPresentationArchiveWrite, UmiAppearanceShortcutPresentationArchiveRead, UmiAppearanceShortcutPresentationArchiveValidate)
