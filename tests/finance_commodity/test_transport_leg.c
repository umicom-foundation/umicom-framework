/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_transport_leg.c
 *
 * PURPOSE:
 *   Implement the test transport leg behavior for
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

#include "umicom/finance/commodity/transport_leg.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/transport_leg.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityTransportLegTransferEqual(const UmiCommodityTransportLeg *a, const UmiCommodityTransportLeg *b)
{
    return strcmp(a->route_id.value, b->route_id.value) == 0 &&
        a->sequence == b->sequence &&
        strcmp(a->origin_location_id.value, b->origin_location_id.value) == 0 &&
        strcmp(a->destination_location_id.value, b->destination_location_id.value) == 0 &&
        a->planned_departure_ms == b->planned_departure_ms &&
        a->planned_arrival_ms == b->planned_arrival_ms;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityTransportLegTransferTails(UmiCommodityTransportLeg *value)
{
    (void)value;
    {
        size_t used = strlen(value->route_id.value) + 1U;
        memset(value->route_id.value + used, 0xa5, sizeof(value->route_id.value) - used);
    }
    {
        size_t used = strlen(value->origin_location_id.value) + 1U;
        memset(value->origin_location_id.value + used, 0xa5, sizeof(value->origin_location_id.value) - used);
    }
    {
        size_t used = strlen(value->destination_location_id.value) + 1U;
        memset(value->destination_location_id.value + used, 0xa5, sizeof(value->destination_location_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityTransportLegTransferMalformed(const UmiCommodityTransportLeg *sample)
{
    (void)sample;
    {
        UmiCommodityTransportLeg invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.route_id.value, 'x', sizeof(invalid.route_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_transport_leg_valid(&invalid)) ||
            umi_commodity_transport_leg_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated route_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityTransportLeg invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.origin_location_id.value, 'x', sizeof(invalid.origin_location_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_transport_leg_valid(&invalid)) ||
            umi_commodity_transport_leg_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated origin_location_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityTransportLeg invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.destination_location_id.value, 'x', sizeof(invalid.destination_location_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_transport_leg_valid(&invalid)) ||
            umi_commodity_transport_leg_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated destination_location_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityTransportLegTransferCases, UmiCommodityTransportLeg,
    umi_commodity_transport_leg_archive_encode, umi_commodity_transport_leg_archive_decode,
    UmiCommodityTransportLegTransferEqual, UmiCommodityTransportLegTransferTails, UmiCommodityTransportLegTransferMalformed)

int main(void)
{
    UmiCommodityTransportLeg value;
    CHECK(umi_commodity_transport_leg_init(&value, "ROUTE-1", 1U, "LOC-A", "LOC-B", 1000, 2000) == UMI_STATUS_OK);
    CHECK(umi_commodity_transport_leg_valid(&value));
    if (UmiCommodityTransportLegTransferCases(&value) != 0) return 1;

    return 0;
}
