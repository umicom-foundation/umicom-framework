/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/tree_binding.c
 *
 * PURPOSE:
 *   Implement tree provider/expansion/selection binding paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/tree_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the tree binding contract to deterministic zero/default state. */
void umi_ui_reactive_tree_binding_init(UmiUiReactiveTreeBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_tree_binding_valid(const UmiUiReactiveTreeBinding *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->tree_id, '\0', sizeof(item->tree_id)) == NULL) return 0;
    if (memchr(item->provider_path, '\0', sizeof(item->provider_path)) == NULL) return 0;
    if (memchr(item->expansion_path, '\0', sizeof(item->expansion_path)) == NULL) return 0;
    if (memchr(item->selection_path, '\0', sizeof(item->selection_path)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveTreeBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x28f9b54e484c2933);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveTreeBinding *)0)->tree_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveTreeBinding *)0)->provider_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveTreeBinding *)0)->expansion_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveTreeBinding *)0)->selection_path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveTreeBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveTreeBinding *)0)->tree_id) - 1U +
        8U + sizeof(((UmiUiReactiveTreeBinding *)0)->provider_path) - 1U +
        8U + sizeof(((UmiUiReactiveTreeBinding *)0)->expansion_path) - 1U +
        8U + sizeof(((UmiUiReactiveTreeBinding *)0)->selection_path) - 1U;
}
static void UmiUiReactiveTreeBindingArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveTreeBinding *value)
{
    UmiArchiveWriteText(writer, value->tree_id, sizeof(value->tree_id));
    UmiArchiveWriteText(writer, value->provider_path, sizeof(value->provider_path));
    UmiArchiveWriteText(writer, value->expansion_path, sizeof(value->expansion_path));
    UmiArchiveWriteText(writer, value->selection_path, sizeof(value->selection_path));
}
static void UmiUiReactiveTreeBindingArchiveRead(UmiArchiveReader *reader, UmiUiReactiveTreeBinding *value)
{
    UmiArchiveReadText(reader, value->tree_id, sizeof(value->tree_id));
    UmiArchiveReadText(reader, value->provider_path, sizeof(value->provider_path));
    UmiArchiveReadText(reader, value->expansion_path, sizeof(value->expansion_path));
    UmiArchiveReadText(reader, value->selection_path, sizeof(value->selection_path));
}
static UmiStatus UmiUiReactiveTreeBindingArchiveValidate(const UmiUiReactiveTreeBinding *value)
{
    return umi_ui_reactive_tree_binding_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_tree_binding_archive_encode, umi_ui_reactive_tree_binding_archive_decode,
    UmiUiReactiveTreeBinding, UmiUiReactiveTreeBindingArchiveSchema, UmiUiReactiveTreeBindingArchiveBound, UmiUiReactiveTreeBindingArchiveWrite, UmiUiReactiveTreeBindingArchiveRead, UmiUiReactiveTreeBindingArchiveValidate)
