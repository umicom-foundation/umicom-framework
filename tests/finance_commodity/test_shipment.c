/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_shipment.c
 *
 * PURPOSE:
 *   Implement the test shipment behavior for
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

#include "umicom/finance/commodity/shipment.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/shipment.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityShipmentTransferEqual(const UmiCommodityShipment *a, const UmiCommodityShipment *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->contract_id.value, b->contract_id.value) == 0 &&
        strcmp(a->route_id.value, b->route_id.value) == 0 &&
        a->planned_quantity.units == b->planned_quantity.units &&
        a->planned_quantity.scale == b->planned_quantity.scale &&
        strcmp(a->planned_quantity.unit_code, b->planned_quantity.unit_code) == 0 &&
        a->delivered_units == b->delivered_units &&
        a->state == b->state;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityShipmentTransferTails(UmiCommodityShipment *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->contract_id.value) + 1U;
        memset(value->contract_id.value + used, 0xa5, sizeof(value->contract_id.value) - used);
    }
    {
        size_t used = strlen(value->route_id.value) + 1U;
        memset(value->route_id.value + used, 0xa5, sizeof(value->route_id.value) - used);
    }
    {
        size_t used = strlen(value->planned_quantity.unit_code) + 1U;
        memset(value->planned_quantity.unit_code + used, 0xa5, sizeof(value->planned_quantity.unit_code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityShipmentTransferMalformed(const UmiCommodityShipment *sample)
{
    (void)sample;
    {
        UmiCommodityShipment invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_shipment_valid(&invalid)) ||
            umi_commodity_shipment_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityShipment invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.contract_id.value, 'x', sizeof(invalid.contract_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_shipment_valid(&invalid)) ||
            umi_commodity_shipment_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated contract_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityShipment invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.route_id.value, 'x', sizeof(invalid.route_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_shipment_valid(&invalid)) ||
            umi_commodity_shipment_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated route_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityShipment invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.planned_quantity.unit_code, 'x', sizeof(invalid.planned_quantity.unit_code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_shipment_valid(&invalid)) ||
            umi_commodity_shipment_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated planned_quantity.unit_code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityShipmentTransferCases, UmiCommodityShipment,
    umi_commodity_shipment_archive_encode, umi_commodity_shipment_archive_decode,
    UmiCommodityShipmentTransferEqual, UmiCommodityShipmentTransferTails, UmiCommodityShipmentTransferMalformed)

int main(void)
{
    UmiCommodityShipment value;
    CHECK(umi_commodity_shipment_init(&value, "SHIP-1", "CTR-1", "ROUTE-1", 500, 0, "MT") == UMI_STATUS_OK);
    CHECK(umi_commodity_shipment_valid(&value));
    if (UmiCommodityShipmentTransferCases(&value) != 0) return 1;

    return 0;
}
