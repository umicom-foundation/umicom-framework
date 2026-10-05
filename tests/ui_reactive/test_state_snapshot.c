/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_state_snapshot.c
 *
 * PURPOSE:
 *   Exercise the state snapshot reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/ui/reactive/state_snapshot.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/state_snapshot.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveStateSnapshotTransferEqual(const UmiUiReactiveStateSnapshot *a, const UmiUiReactiveStateSnapshot *b)
{
    return strcmp(a->snapshot_id, b->snapshot_id) == 0 &&
        a->revision == b->revision &&
        a->property_count == b->property_count &&
        a->fingerprint == b->fingerprint;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveStateSnapshotTransferTails(UmiUiReactiveStateSnapshot *value)
{
    (void)value;
    {
        size_t used = strlen(value->snapshot_id) + 1U;
        memset(value->snapshot_id + used, 0xa5, sizeof(value->snapshot_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveStateSnapshotTransferMalformed(const UmiUiReactiveStateSnapshot *sample)
{
    (void)sample;
    {
        UmiUiReactiveStateSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.snapshot_id, 'x', sizeof(invalid.snapshot_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_state_snapshot_valid(&invalid)) ||
            umi_ui_reactive_state_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated snapshot_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveStateSnapshotTransferCases, UmiUiReactiveStateSnapshot,
    umi_ui_reactive_state_snapshot_archive_encode, umi_ui_reactive_state_snapshot_archive_decode,
    UmiUiReactiveStateSnapshotTransferEqual, UmiUiReactiveStateSnapshotTransferTails, UmiUiReactiveStateSnapshotTransferMalformed)

int main(void) { UmiUiReactiveStateSnapshot item; umi_ui_reactive_state_snapshot_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveStateSnapshot populated = item;
    (void)snprintf(populated.snapshot_id, sizeof(populated.snapshot_id), "field-0");
    populated.revision = (uint64_t)3;
    populated.property_count = (size_t)4;
    populated.fingerprint = (uint64_t)5;
    if (UmiUiReactiveStateSnapshotTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_state_snapshot_valid(&item) ? 0 : 1; }
