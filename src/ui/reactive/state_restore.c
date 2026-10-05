/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/state_restore.c
 *
 * PURPOSE:
 *   Implement governed state restoration intent and result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/state_restore.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the state restore contract to deterministic zero/default state. */
void umi_ui_reactive_state_restore_init(UmiUiReactiveStateRestore *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_state_restore_valid(const UmiUiReactiveStateRestore *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->snapshot_id, '\0', sizeof(item->snapshot_id)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveStateRestoreArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc6406fe65f1a8578);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveStateRestore *)0)->snapshot_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveStateRestoreArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveStateRestore *)0)->snapshot_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveStateRestoreArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveStateRestore *value)
{
    UmiArchiveWriteText(writer, value->snapshot_id, sizeof(value->snapshot_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->from_revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->to_revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->completed);
}
static void UmiUiReactiveStateRestoreArchiveRead(UmiArchiveReader *reader, UmiUiReactiveStateRestore *value)
{
    UmiArchiveReadText(reader, value->snapshot_id, sizeof(value->snapshot_id));
    value->from_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->to_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->completed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveStateRestoreArchiveValidate(const UmiUiReactiveStateRestore *value)
{
    return umi_ui_reactive_state_restore_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_state_restore_archive_encode, umi_ui_reactive_state_restore_archive_decode,
    UmiUiReactiveStateRestore, UmiUiReactiveStateRestoreArchiveSchema, UmiUiReactiveStateRestoreArchiveBound, UmiUiReactiveStateRestoreArchiveWrite, UmiUiReactiveStateRestoreArchiveRead, UmiUiReactiveStateRestoreArchiveValidate)
