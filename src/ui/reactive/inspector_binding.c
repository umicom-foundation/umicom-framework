/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/inspector_binding.c
 *
 * PURPOSE:
 *   Implement inspector subject and editing binding paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/inspector_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the inspector binding contract to deterministic zero/default state. */
void umi_ui_reactive_inspector_binding_init(UmiUiReactiveInspectorBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_inspector_binding_valid(const UmiUiReactiveInspectorBinding *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->inspector_id, '\0', sizeof(item->inspector_id)) == NULL) return 0;
    if (memchr(item->subject_path, '\0', sizeof(item->subject_path)) == NULL) return 0;
    if (memchr(item->edit_path, '\0', sizeof(item->edit_path)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveInspectorBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb844da475809c6d4);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveInspectorBinding *)0)->inspector_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveInspectorBinding *)0)->subject_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveInspectorBinding *)0)->edit_path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveInspectorBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveInspectorBinding *)0)->inspector_id) - 1U +
        8U + sizeof(((UmiUiReactiveInspectorBinding *)0)->subject_path) - 1U +
        8U + sizeof(((UmiUiReactiveInspectorBinding *)0)->edit_path) - 1U;
}
static void UmiUiReactiveInspectorBindingArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveInspectorBinding *value)
{
    UmiArchiveWriteText(writer, value->inspector_id, sizeof(value->inspector_id));
    UmiArchiveWriteText(writer, value->subject_path, sizeof(value->subject_path));
    UmiArchiveWriteText(writer, value->edit_path, sizeof(value->edit_path));
}
static void UmiUiReactiveInspectorBindingArchiveRead(UmiArchiveReader *reader, UmiUiReactiveInspectorBinding *value)
{
    UmiArchiveReadText(reader, value->inspector_id, sizeof(value->inspector_id));
    UmiArchiveReadText(reader, value->subject_path, sizeof(value->subject_path));
    UmiArchiveReadText(reader, value->edit_path, sizeof(value->edit_path));
}
static UmiStatus UmiUiReactiveInspectorBindingArchiveValidate(const UmiUiReactiveInspectorBinding *value)
{
    return umi_ui_reactive_inspector_binding_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_inspector_binding_archive_encode, umi_ui_reactive_inspector_binding_archive_decode,
    UmiUiReactiveInspectorBinding, UmiUiReactiveInspectorBindingArchiveSchema, UmiUiReactiveInspectorBindingArchiveBound, UmiUiReactiveInspectorBindingArchiveWrite, UmiUiReactiveInspectorBindingArchiveRead, UmiUiReactiveInspectorBindingArchiveValidate)
