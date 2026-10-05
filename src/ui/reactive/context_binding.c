/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/context_binding.c
 *
 * PURPOSE:
 *   Synchronise typed context channels with declarative UI properties.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/context_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the context binding contract to deterministic zero/default state. */
void umi_ui_reactive_context_binding_init(UmiUiReactiveContextBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_context_binding_valid(const UmiUiReactiveContextBinding *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->channel_id, '\0', sizeof(item->channel_id)) == NULL) return 0;
    if (memchr(item->property_path, '\0', sizeof(item->property_path)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveContextBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0186bb6547e415b9);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveContextBinding *)0)->channel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveContextBinding *)0)->property_path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveContextBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveContextBinding *)0)->channel_id) - 1U +
        8U + sizeof(((UmiUiReactiveContextBinding *)0)->property_path) - 1U +
        8U +
        8U;
}
static void UmiUiReactiveContextBindingArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveContextBinding *value)
{
    UmiArchiveWriteText(writer, value->channel_id, sizeof(value->channel_id));
    UmiArchiveWriteText(writer, value->property_path, sizeof(value->property_path));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->publish);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->subscribe);
}
static void UmiUiReactiveContextBindingArchiveRead(UmiArchiveReader *reader, UmiUiReactiveContextBinding *value)
{
    UmiArchiveReadText(reader, value->channel_id, sizeof(value->channel_id));
    UmiArchiveReadText(reader, value->property_path, sizeof(value->property_path));
    value->publish = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->subscribe = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveContextBindingArchiveValidate(const UmiUiReactiveContextBinding *value)
{
    return umi_ui_reactive_context_binding_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_context_binding_archive_encode, umi_ui_reactive_context_binding_archive_decode,
    UmiUiReactiveContextBinding, UmiUiReactiveContextBindingArchiveSchema, UmiUiReactiveContextBindingArchiveBound, UmiUiReactiveContextBindingArchiveWrite, UmiUiReactiveContextBindingArchiveRead, UmiUiReactiveContextBindingArchiveValidate)
