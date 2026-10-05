/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/state_snapshot.c
 *
 * PURPOSE:
 *   Implement a named immutable state snapshot reference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/state_snapshot.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the state snapshot contract to deterministic zero/default state. */
void umi_ui_reactive_state_snapshot_init(UmiUiReactiveStateSnapshot *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_state_snapshot_valid(const UmiUiReactiveStateSnapshot *item) {
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
static uint64_t UmiUiReactiveStateSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb98018ab43aa2df6);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveStateSnapshot *)0)->snapshot_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveStateSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveStateSnapshot *)0)->snapshot_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveStateSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveStateSnapshot *value)
{
    UmiArchiveWriteText(writer, value->snapshot_id, sizeof(value->snapshot_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->property_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->fingerprint);
}
static void UmiUiReactiveStateSnapshotArchiveRead(UmiArchiveReader *reader, UmiUiReactiveStateSnapshot *value)
{
    UmiArchiveReadText(reader, value->snapshot_id, sizeof(value->snapshot_id));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->property_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiUiReactiveStateSnapshotArchiveValidate(const UmiUiReactiveStateSnapshot *value)
{
    return umi_ui_reactive_state_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_state_snapshot_archive_encode, umi_ui_reactive_state_snapshot_archive_decode,
    UmiUiReactiveStateSnapshot, UmiUiReactiveStateSnapshotArchiveSchema, UmiUiReactiveStateSnapshotArchiveBound, UmiUiReactiveStateSnapshotArchiveWrite, UmiUiReactiveStateSnapshotArchiveRead, UmiUiReactiveStateSnapshotArchiveValidate)
