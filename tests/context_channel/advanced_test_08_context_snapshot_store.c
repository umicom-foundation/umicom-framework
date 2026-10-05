/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_08_context_snapshot_store.c
 *
 * PURPOSE:
 *   Validate context snapshot store sequence accounting, bounded fields and failure evidence.
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
#include <assert.h>
#include <string.h>
#include "umicom/context_channel/context_snapshot_store.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_snapshot_store.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextSnapshotStoreTransferEqual(const UmiContextSnapshotStore *a, const UmiContextSnapshotStore *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->snapshot_id, b->snapshot_id) == 0 &&
        strcmp(a->context_id, b->context_id) == 0 &&
        strcmp(a->channel_id, b->channel_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        a->first_sequence == b->first_sequence &&
        a->last_sequence == b->last_sequence &&
        a->item_count == b->item_count &&
        a->failure_count == b->failure_count &&
        a->status == b->status &&
        a->enabled == b->enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextSnapshotStoreTransferTails(UmiContextSnapshotStore *value)
{
    (void)value;
    {
        size_t used = strlen(value->snapshot_id) + 1U;
        memset(value->snapshot_id + used, 0xa5, sizeof(value->snapshot_id) - used);
    }
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
    {
        size_t used = strlen(value->channel_id) + 1U;
        memset(value->channel_id + used, 0xa5, sizeof(value->channel_id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextSnapshotStoreTransferMalformed(const UmiContextSnapshotStore *sample)
{
    (void)sample;
    {
        UmiContextSnapshotStore invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.snapshot_id, 'x', sizeof(invalid.snapshot_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_snapshot_store_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_snapshot_store_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated snapshot_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSnapshotStore invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_snapshot_store_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_snapshot_store_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSnapshotStore invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.channel_id, 'x', sizeof(invalid.channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_snapshot_store_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_snapshot_store_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSnapshotStore invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_snapshot_store_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_snapshot_store_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextSnapshotStoreTransferCases, UmiContextSnapshotStore,
    umi_context_snapshot_store_archive_encode, umi_context_snapshot_store_archive_decode,
    UmiContextSnapshotStoreTransferEqual, UmiContextSnapshotStoreTransferTails, UmiContextSnapshotStoreTransferMalformed)

int main(void)
{
    UmiContextSnapshotStore state;
    umi_context_snapshot_store_init(&state);
    assert(umi_context_snapshot_store_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_snapshot_store_field(&state,0U),"alpha") == 0);
    assert(umi_context_snapshot_store_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_snapshot_store_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_snapshot_store_covers_sequence(&state,10U));
    assert(umi_context_snapshot_store_covers_sequence(&state,11U));
    assert(umi_context_snapshot_store_validate(&state) == UMI_STATUS_OK);
    if (UmiContextSnapshotStoreTransferCases(&state) != 0) return 1;

    return 0;
}
