/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/command_enablement.c
 *
 * PURPOSE:
 *   Implement command enablement evidence from a state expression.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/command_enablement.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the command enablement contract to deterministic zero/default state. */
void umi_ui_reactive_command_enablement_init(UmiUiReactiveCommandEnablement *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_command_enablement_valid(const UmiUiReactiveCommandEnablement *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->command_id, '\0', sizeof(item->command_id)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveCommandEnablementArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x060f1a6e9c9746eb);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveCommandEnablement *)0)->command_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveCommandEnablementArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveCommandEnablement *)0)->command_id) - 1U +
        8U +
        8U;
}
static void UmiUiReactiveCommandEnablementArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveCommandEnablement *value)
{
    UmiArchiveWriteText(writer, value->command_id, sizeof(value->command_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->evaluation_revision);
}
static void UmiUiReactiveCommandEnablementArchiveRead(UmiArchiveReader *reader, UmiUiReactiveCommandEnablement *value)
{
    UmiArchiveReadText(reader, value->command_id, sizeof(value->command_id));
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->evaluation_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiUiReactiveCommandEnablementArchiveValidate(const UmiUiReactiveCommandEnablement *value)
{
    return umi_ui_reactive_command_enablement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_command_enablement_archive_encode, umi_ui_reactive_command_enablement_archive_decode,
    UmiUiReactiveCommandEnablement, UmiUiReactiveCommandEnablementArchiveSchema, UmiUiReactiveCommandEnablementArchiveBound, UmiUiReactiveCommandEnablementArchiveWrite, UmiUiReactiveCommandEnablementArchiveRead, UmiUiReactiveCommandEnablementArchiveValidate)
