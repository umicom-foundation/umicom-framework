/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/command_binding.c
 *
 * PURPOSE:
 *   Connect a semantic command to reactive enablement and parameter state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/command_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the command binding contract to deterministic zero/default state. */
void umi_ui_reactive_command_binding_init(UmiUiReactiveCommandBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_command_binding_valid(const UmiUiReactiveCommandBinding *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->binding_id, '\0', sizeof(item->binding_id)) == NULL) return 0;
    if (memchr(item->command_id, '\0', sizeof(item->command_id)) == NULL) return 0;
    if (memchr(item->parameter_path, '\0', sizeof(item->parameter_path)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveCommandBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x38346cb6c5e858bf);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveCommandBinding *)0)->binding_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveCommandBinding *)0)->command_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveCommandBinding *)0)->parameter_path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveCommandBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveCommandBinding *)0)->binding_id) - 1U +
        8U + sizeof(((UmiUiReactiveCommandBinding *)0)->command_id) - 1U +
        8U + sizeof(((UmiUiReactiveCommandBinding *)0)->parameter_path) - 1U +
        8U;
}
static void UmiUiReactiveCommandBindingArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveCommandBinding *value)
{
    UmiArchiveWriteText(writer, value->binding_id, sizeof(value->binding_id));
    UmiArchiveWriteText(writer, value->command_id, sizeof(value->command_id));
    UmiArchiveWriteText(writer, value->parameter_path, sizeof(value->parameter_path));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiUiReactiveCommandBindingArchiveRead(UmiArchiveReader *reader, UmiUiReactiveCommandBinding *value)
{
    UmiArchiveReadText(reader, value->binding_id, sizeof(value->binding_id));
    UmiArchiveReadText(reader, value->command_id, sizeof(value->command_id));
    UmiArchiveReadText(reader, value->parameter_path, sizeof(value->parameter_path));
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveCommandBindingArchiveValidate(const UmiUiReactiveCommandBinding *value)
{
    return umi_ui_reactive_command_binding_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_command_binding_archive_encode, umi_ui_reactive_command_binding_archive_decode,
    UmiUiReactiveCommandBinding, UmiUiReactiveCommandBindingArchiveSchema, UmiUiReactiveCommandBindingArchiveBound, UmiUiReactiveCommandBindingArchiveWrite, UmiUiReactiveCommandBindingArchiveRead, UmiUiReactiveCommandBindingArchiveValidate)
