/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/binding_endpoint.c
 *
 * PURPOSE:
 *   Implement a view/property endpoint without owning the target object.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_endpoint.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the binding endpoint contract to deterministic zero/default state. */
void umi_ui_reactive_binding_endpoint_init(UmiUiReactiveBindingEndpoint *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_binding_endpoint_valid(const UmiUiReactiveBindingEndpoint *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->view_id, '\0', sizeof(item->view_id)) == NULL) return 0;
    if (memchr(item->property_path, '\0', sizeof(item->property_path)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveBindingEndpointArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe0edbb5aa8cc41cf);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveBindingEndpoint *)0)->view_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveBindingEndpoint *)0)->property_path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveBindingEndpointArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveBindingEndpoint *)0)->view_id) - 1U +
        8U + sizeof(((UmiUiReactiveBindingEndpoint *)0)->property_path) - 1U +
        8U;
}
static void UmiUiReactiveBindingEndpointArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveBindingEndpoint *value)
{
    UmiArchiveWriteText(writer, value->view_id, sizeof(value->view_id));
    UmiArchiveWriteText(writer, value->property_path, sizeof(value->property_path));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->writable);
}
static void UmiUiReactiveBindingEndpointArchiveRead(UmiArchiveReader *reader, UmiUiReactiveBindingEndpoint *value)
{
    UmiArchiveReadText(reader, value->view_id, sizeof(value->view_id));
    UmiArchiveReadText(reader, value->property_path, sizeof(value->property_path));
    value->writable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveBindingEndpointArchiveValidate(const UmiUiReactiveBindingEndpoint *value)
{
    return umi_ui_reactive_binding_endpoint_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_binding_endpoint_archive_encode, umi_ui_reactive_binding_endpoint_archive_decode,
    UmiUiReactiveBindingEndpoint, UmiUiReactiveBindingEndpointArchiveSchema, UmiUiReactiveBindingEndpointArchiveBound, UmiUiReactiveBindingEndpointArchiveWrite, UmiUiReactiveBindingEndpointArchiveRead, UmiUiReactiveBindingEndpointArchiveValidate)
