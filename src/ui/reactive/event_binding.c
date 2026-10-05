/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/event_binding.c
 *
 * PURPOSE:
 *   Route a semantic UI event to a command or state action.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/event_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the event binding contract to deterministic zero/default state. */
void umi_ui_reactive_event_binding_init(UmiUiReactiveEventBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_event_binding_valid(const UmiUiReactiveEventBinding *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->source_id, '\0', sizeof(item->source_id)) == NULL) return 0;
    if (memchr(item->event_name, '\0', sizeof(item->event_name)) == NULL) return 0;
    if (memchr(item->action_id, '\0', sizeof(item->action_id)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveEventBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x709adbf39894bde0);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveEventBinding *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveEventBinding *)0)->event_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveEventBinding *)0)->action_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveEventBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveEventBinding *)0)->source_id) - 1U +
        8U + sizeof(((UmiUiReactiveEventBinding *)0)->event_name) - 1U +
        8U + sizeof(((UmiUiReactiveEventBinding *)0)->action_id) - 1U +
        8U;
}
static void UmiUiReactiveEventBindingArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveEventBinding *value)
{
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->event_name, sizeof(value->event_name));
    UmiArchiveWriteText(writer, value->action_id, sizeof(value->action_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiUiReactiveEventBindingArchiveRead(UmiArchiveReader *reader, UmiUiReactiveEventBinding *value)
{
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->event_name, sizeof(value->event_name));
    UmiArchiveReadText(reader, value->action_id, sizeof(value->action_id));
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveEventBindingArchiveValidate(const UmiUiReactiveEventBinding *value)
{
    return umi_ui_reactive_event_binding_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_event_binding_archive_encode, umi_ui_reactive_event_binding_archive_decode,
    UmiUiReactiveEventBinding, UmiUiReactiveEventBindingArchiveSchema, UmiUiReactiveEventBindingArchiveBound, UmiUiReactiveEventBindingArchiveWrite, UmiUiReactiveEventBindingArchiveRead, UmiUiReactiveEventBindingArchiveValidate)
