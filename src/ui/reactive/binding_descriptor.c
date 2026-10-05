/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/binding_descriptor.c
 *
 * PURPOSE:
 *   Implement a declarative binding between source and target endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the binding descriptor contract to deterministic zero/default state. */
void umi_ui_reactive_binding_descriptor_init(UmiUiReactiveBindingDescriptor *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_binding_descriptor_valid(const UmiUiReactiveBindingDescriptor *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->binding_id, '\0', sizeof(item->binding_id)) == NULL) return 0;
    if (memchr(item->source_path, '\0', sizeof(item->source_path)) == NULL) return 0;
    if (memchr(item->target_path, '\0', sizeof(item->target_path)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveBindingDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0575978433b02e62);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveBindingDescriptor *)0)->binding_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveBindingDescriptor *)0)->source_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveBindingDescriptor *)0)->target_path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveBindingDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveBindingDescriptor *)0)->binding_id) - 1U +
        8U + sizeof(((UmiUiReactiveBindingDescriptor *)0)->source_path) - 1U +
        8U + sizeof(((UmiUiReactiveBindingDescriptor *)0)->target_path) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveBindingDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveBindingDescriptor *value)
{
    UmiArchiveWriteText(writer, value->binding_id, sizeof(value->binding_id));
    UmiArchiveWriteText(writer, value->source_path, sizeof(value->source_path));
    UmiArchiveWriteText(writer, value->target_path, sizeof(value->target_path));
    UmiArchiveWriteSigned(writer, (int64_t)value->direction);
    UmiArchiveWriteSigned(writer, (int64_t)value->trigger);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiUiReactiveBindingDescriptorArchiveRead(UmiArchiveReader *reader, UmiUiReactiveBindingDescriptor *value)
{
    UmiArchiveReadText(reader, value->binding_id, sizeof(value->binding_id));
    UmiArchiveReadText(reader, value->source_path, sizeof(value->source_path));
    UmiArchiveReadText(reader, value->target_path, sizeof(value->target_path));
    value->direction = (UmiUiReactiveBindingDirection)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->trigger = (UmiUiReactiveUpdateTrigger)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveBindingDescriptorArchiveValidate(const UmiUiReactiveBindingDescriptor *value)
{
    return umi_ui_reactive_binding_descriptor_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_binding_descriptor_archive_encode, umi_ui_reactive_binding_descriptor_archive_decode,
    UmiUiReactiveBindingDescriptor, UmiUiReactiveBindingDescriptorArchiveSchema, UmiUiReactiveBindingDescriptorArchiveBound, UmiUiReactiveBindingDescriptorArchiveWrite, UmiUiReactiveBindingDescriptorArchiveRead, UmiUiReactiveBindingDescriptorArchiveValidate)
