/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_connection_slot.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the connection slot enterprise data capability.
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
#include "umicom/data/enterprise/connection_slot.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/connection_slot.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataConnectionSlotTransferEqual(const UmiDataConnectionSlot *a, const UmiDataConnectionSlot *b)
{
    return strcmp(a->slot_id, b->slot_id) == 0 &&
        a->connection_token == b->connection_token &&
        a->last_used_at == b->last_used_at &&
        a->lease_count == b->lease_count &&
        a->healthy == b->healthy &&
        a->leased == b->leased;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataConnectionSlotTransferTails(UmiDataConnectionSlot *value)
{
    (void)value;
    {
        size_t used = strlen(value->slot_id) + 1U;
        memset(value->slot_id + used, 0xa5, sizeof(value->slot_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataConnectionSlotTransferMalformed(const UmiDataConnectionSlot *sample)
{
    (void)sample;
    {
        UmiDataConnectionSlot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.slot_id, 'x', sizeof(invalid.slot_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_connection_slot_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_connection_slot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated slot_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataConnectionSlotTransferCases, UmiDataConnectionSlot,
    umi_data_connection_slot_archive_encode, umi_data_connection_slot_archive_decode,
    UmiDataConnectionSlotTransferEqual, UmiDataConnectionSlotTransferTails, UmiDataConnectionSlotTransferMalformed)

int main(void) {
    UmiDataConnectionSlot item;
    CHECK(umi_data_connection_slot_init(&item,"slot1",100U) == UMI_STATUS_OK);
    if (UmiDataConnectionSlotTransferCases(&item) != 0) return 1;

    CHECK(item.healthy && !item.leased);
    return 0;
}
