/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_types.c
 *
 * PURPOSE:
 *   Verify Open TMS input defaults and snapshot validation.
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
#include "umicom/open_tms/types.h"
#include "../value_archive/transfer_cases.h"

#include "umicom/open_tms/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiOpenTmsSnapshotTransferEqual(const UmiOpenTmsSnapshot *a, const UmiOpenTmsSnapshot *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        a->value == b->value &&
        a->secondaryValue == b->secondaryValue &&
        a->score == b->score &&
        a->ratio == b->ratio &&
        a->pnl == b->pnl &&
        a->ready == b->ready &&
        a->attention == b->attention &&
        a->blocked == b->blocked &&
        a->approvalRequired == b->approvalRequired &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiOpenTmsSnapshotTransferTails(UmiOpenTmsSnapshot *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiOpenTmsSnapshotTransferMalformed(const UmiOpenTmsSnapshot *sample)
{
    (void)sample;
    {
        UmiOpenTmsSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_open_tms_snapshot_validate(&invalid) != UMI_STATUS_OK) ||
            umi_open_tms_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiOpenTmsSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_open_tms_snapshot_validate(&invalid) != UMI_STATUS_OK) ||
            umi_open_tms_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiOpenTmsSnapshotTransferCases, UmiOpenTmsSnapshot,
    umi_open_tms_snapshot_archive_encode, umi_open_tms_snapshot_archive_decode,
    UmiOpenTmsSnapshotTransferEqual, UmiOpenTmsSnapshotTransferTails, UmiOpenTmsSnapshotTransferMalformed)

int main(void)
{
    UmiOpenTmsInput input; UmiOpenTmsSnapshot snapshot;
    umi_open_tms_input_init(&input); assert(input.trusted==1);
    umi_open_tms_snapshot_init(&snapshot);
    (void)strcpy(snapshot.id,"cash.forecast-net"); snapshot.score=75.0; snapshot.ready=1;
    assert(umi_open_tms_snapshot_validate(&snapshot)==UMI_STATUS_OK);
    if (UmiOpenTmsSnapshotTransferCases(&snapshot) != 0) return 1;

    snapshot.score=101.0;
    assert(umi_open_tms_snapshot_validate(&snapshot)==UMI_STATUS_INVALID_STATE);
    return 0;
}
