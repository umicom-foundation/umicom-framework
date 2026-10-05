/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_inventory_lot.c
 *
 * PURPOSE:
 *   Implement the test inventory lot behavior for
 *   Umicom Framework.
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
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/inventory_lot.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/inventory_lot.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityInventoryLotTransferEqual(const UmiCommodityInventoryLot *a, const UmiCommodityInventoryLot *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->commodity_id.value, b->commodity_id.value) == 0 &&
        strcmp(a->facility_id.value, b->facility_id.value) == 0 &&
        a->quantity.units == b->quantity.units &&
        a->quantity.scale == b->quantity.scale &&
        strcmp(a->quantity.unit_code, b->quantity.unit_code) == 0 &&
        a->received_time_ms == b->received_time_ms &&
        a->quality_accepted == b->quality_accepted;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityInventoryLotTransferTails(UmiCommodityInventoryLot *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->commodity_id.value) + 1U;
        memset(value->commodity_id.value + used, 0xa5, sizeof(value->commodity_id.value) - used);
    }
    {
        size_t used = strlen(value->facility_id.value) + 1U;
        memset(value->facility_id.value + used, 0xa5, sizeof(value->facility_id.value) - used);
    }
    {
        size_t used = strlen(value->quantity.unit_code) + 1U;
        memset(value->quantity.unit_code + used, 0xa5, sizeof(value->quantity.unit_code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityInventoryLotTransferMalformed(const UmiCommodityInventoryLot *sample)
{
    (void)sample;
    {
        UmiCommodityInventoryLot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_inventory_lot_valid(&invalid)) ||
            umi_commodity_inventory_lot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityInventoryLot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.commodity_id.value, 'x', sizeof(invalid.commodity_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_inventory_lot_valid(&invalid)) ||
            umi_commodity_inventory_lot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated commodity_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityInventoryLot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.facility_id.value, 'x', sizeof(invalid.facility_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_inventory_lot_valid(&invalid)) ||
            umi_commodity_inventory_lot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated facility_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityInventoryLot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.quantity.unit_code, 'x', sizeof(invalid.quantity.unit_code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_inventory_lot_valid(&invalid)) ||
            umi_commodity_inventory_lot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated quantity.unit_code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityInventoryLotTransferCases, UmiCommodityInventoryLot,
    umi_commodity_inventory_lot_archive_encode, umi_commodity_inventory_lot_archive_decode,
    UmiCommodityInventoryLotTransferEqual, UmiCommodityInventoryLotTransferTails, UmiCommodityInventoryLotTransferMalformed)

int main(void)
{
    UmiCommodityInventoryLot value;
    CHECK(umi_commodity_inventory_lot_init(&value, "LOT-1", "CMD-WTI", "FAC-1", 5000, 0, "BBL", 1000) == UMI_STATUS_OK);
    CHECK(umi_commodity_inventory_lot_valid(&value));
    if (UmiCommodityInventoryLotTransferCases(&value) != 0) return 1;

    return 0;
}
