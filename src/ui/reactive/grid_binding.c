/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/grid_binding.c
 *
 * PURPOSE:
 *   Implement enterprise grid provider/query/selection binding paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/grid_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the grid binding contract to deterministic zero/default state. */
void umi_ui_reactive_grid_binding_init(UmiUiReactiveGridBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_grid_binding_valid(const UmiUiReactiveGridBinding *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->grid_id, '\0', sizeof(item->grid_id)) == NULL) return 0;
    if (memchr(item->provider_path, '\0', sizeof(item->provider_path)) == NULL) return 0;
    if (memchr(item->query_path, '\0', sizeof(item->query_path)) == NULL) return 0;
    if (memchr(item->selection_path, '\0', sizeof(item->selection_path)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveGridBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x50922d7210a46923);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveGridBinding *)0)->grid_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveGridBinding *)0)->provider_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveGridBinding *)0)->query_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveGridBinding *)0)->selection_path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveGridBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveGridBinding *)0)->grid_id) - 1U +
        8U + sizeof(((UmiUiReactiveGridBinding *)0)->provider_path) - 1U +
        8U + sizeof(((UmiUiReactiveGridBinding *)0)->query_path) - 1U +
        8U + sizeof(((UmiUiReactiveGridBinding *)0)->selection_path) - 1U;
}
static void UmiUiReactiveGridBindingArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveGridBinding *value)
{
    UmiArchiveWriteText(writer, value->grid_id, sizeof(value->grid_id));
    UmiArchiveWriteText(writer, value->provider_path, sizeof(value->provider_path));
    UmiArchiveWriteText(writer, value->query_path, sizeof(value->query_path));
    UmiArchiveWriteText(writer, value->selection_path, sizeof(value->selection_path));
}
static void UmiUiReactiveGridBindingArchiveRead(UmiArchiveReader *reader, UmiUiReactiveGridBinding *value)
{
    UmiArchiveReadText(reader, value->grid_id, sizeof(value->grid_id));
    UmiArchiveReadText(reader, value->provider_path, sizeof(value->provider_path));
    UmiArchiveReadText(reader, value->query_path, sizeof(value->query_path));
    UmiArchiveReadText(reader, value->selection_path, sizeof(value->selection_path));
}
static UmiStatus UmiUiReactiveGridBindingArchiveValidate(const UmiUiReactiveGridBinding *value)
{
    return umi_ui_reactive_grid_binding_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_grid_binding_archive_encode, umi_ui_reactive_grid_binding_archive_decode,
    UmiUiReactiveGridBinding, UmiUiReactiveGridBindingArchiveSchema, UmiUiReactiveGridBindingArchiveBound, UmiUiReactiveGridBindingArchiveWrite, UmiUiReactiveGridBindingArchiveRead, UmiUiReactiveGridBindingArchiveValidate)
