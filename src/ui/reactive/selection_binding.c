/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/selection_binding.c
 *
 * PURPOSE:
 *   Synchronise canonical selection context with a semantic surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/selection_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the selection binding contract to deterministic zero/default state. */
void umi_ui_reactive_selection_binding_init(UmiUiReactiveSelectionBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_selection_binding_valid(const UmiUiReactiveSelectionBinding *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->surface_id, '\0', sizeof(item->surface_id)) == NULL) return 0;
    if (memchr(item->selection_path, '\0', sizeof(item->selection_path)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveSelectionBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6cf02530eb6e4f91);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveSelectionBinding *)0)->surface_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveSelectionBinding *)0)->selection_path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveSelectionBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveSelectionBinding *)0)->surface_id) - 1U +
        8U + sizeof(((UmiUiReactiveSelectionBinding *)0)->selection_path) - 1U +
        8U;
}
static void UmiUiReactiveSelectionBindingArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveSelectionBinding *value)
{
    UmiArchiveWriteText(writer, value->surface_id, sizeof(value->surface_id));
    UmiArchiveWriteText(writer, value->selection_path, sizeof(value->selection_path));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->two_way);
}
static void UmiUiReactiveSelectionBindingArchiveRead(UmiArchiveReader *reader, UmiUiReactiveSelectionBinding *value)
{
    UmiArchiveReadText(reader, value->surface_id, sizeof(value->surface_id));
    UmiArchiveReadText(reader, value->selection_path, sizeof(value->selection_path));
    value->two_way = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveSelectionBindingArchiveValidate(const UmiUiReactiveSelectionBinding *value)
{
    return umi_ui_reactive_selection_binding_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_selection_binding_archive_encode, umi_ui_reactive_selection_binding_archive_decode,
    UmiUiReactiveSelectionBinding, UmiUiReactiveSelectionBindingArchiveSchema, UmiUiReactiveSelectionBindingArchiveBound, UmiUiReactiveSelectionBindingArchiveWrite, UmiUiReactiveSelectionBindingArchiveRead, UmiUiReactiveSelectionBindingArchiveValidate)
