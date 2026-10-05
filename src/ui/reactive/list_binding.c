/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/list_binding.c
 *
 * PURPOSE:
 *   Implement list data-path and selection-path binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/list_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the list binding contract to deterministic zero/default state. */
void umi_ui_reactive_list_binding_init(UmiUiReactiveListBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_list_binding_valid(const UmiUiReactiveListBinding *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->surface_id, '\0', sizeof(item->surface_id)) == NULL) return 0;
    if (memchr(item->items_path, '\0', sizeof(item->items_path)) == NULL) return 0;
    if (memchr(item->selection_path, '\0', sizeof(item->selection_path)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveListBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x46a17312c4e1d631);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveListBinding *)0)->surface_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveListBinding *)0)->items_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveListBinding *)0)->selection_path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveListBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveListBinding *)0)->surface_id) - 1U +
        8U + sizeof(((UmiUiReactiveListBinding *)0)->items_path) - 1U +
        8U + sizeof(((UmiUiReactiveListBinding *)0)->selection_path) - 1U;
}
static void UmiUiReactiveListBindingArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveListBinding *value)
{
    UmiArchiveWriteText(writer, value->surface_id, sizeof(value->surface_id));
    UmiArchiveWriteText(writer, value->items_path, sizeof(value->items_path));
    UmiArchiveWriteText(writer, value->selection_path, sizeof(value->selection_path));
}
static void UmiUiReactiveListBindingArchiveRead(UmiArchiveReader *reader, UmiUiReactiveListBinding *value)
{
    UmiArchiveReadText(reader, value->surface_id, sizeof(value->surface_id));
    UmiArchiveReadText(reader, value->items_path, sizeof(value->items_path));
    UmiArchiveReadText(reader, value->selection_path, sizeof(value->selection_path));
}
static UmiStatus UmiUiReactiveListBindingArchiveValidate(const UmiUiReactiveListBinding *value)
{
    return umi_ui_reactive_list_binding_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_list_binding_archive_encode, umi_ui_reactive_list_binding_archive_decode,
    UmiUiReactiveListBinding, UmiUiReactiveListBindingArchiveSchema, UmiUiReactiveListBindingArchiveBound, UmiUiReactiveListBindingArchiveWrite, UmiUiReactiveListBindingArchiveRead, UmiUiReactiveListBindingArchiveValidate)
