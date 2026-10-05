/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_transport_route.c
 *
 * PURPOSE:
 *   Implement the test transport route behavior for
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

#include "umicom/finance/commodity/transport_route.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/transport_route.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityTransportRouteTransferEqual(const UmiCommodityTransportRoute *a, const UmiCommodityTransportRoute *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->origin_location_id.value, b->origin_location_id.value) == 0 &&
        strcmp(a->destination_location_id.value, b->destination_location_id.value) == 0 &&
        strcmp(a->mode_code, b->mode_code) == 0 &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityTransportRouteTransferTails(UmiCommodityTransportRoute *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->origin_location_id.value) + 1U;
        memset(value->origin_location_id.value + used, 0xa5, sizeof(value->origin_location_id.value) - used);
    }
    {
        size_t used = strlen(value->destination_location_id.value) + 1U;
        memset(value->destination_location_id.value + used, 0xa5, sizeof(value->destination_location_id.value) - used);
    }
    {
        size_t used = strlen(value->mode_code) + 1U;
        memset(value->mode_code + used, 0xa5, sizeof(value->mode_code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityTransportRouteTransferMalformed(const UmiCommodityTransportRoute *sample)
{
    (void)sample;
    {
        UmiCommodityTransportRoute invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_transport_route_valid(&invalid)) ||
            umi_commodity_transport_route_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityTransportRoute invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.origin_location_id.value, 'x', sizeof(invalid.origin_location_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_transport_route_valid(&invalid)) ||
            umi_commodity_transport_route_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated origin_location_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityTransportRoute invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.destination_location_id.value, 'x', sizeof(invalid.destination_location_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_transport_route_valid(&invalid)) ||
            umi_commodity_transport_route_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated destination_location_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityTransportRoute invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.mode_code, 'x', sizeof(invalid.mode_code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_transport_route_valid(&invalid)) ||
            umi_commodity_transport_route_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated mode_code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityTransportRouteTransferCases, UmiCommodityTransportRoute,
    umi_commodity_transport_route_archive_encode, umi_commodity_transport_route_archive_decode,
    UmiCommodityTransportRouteTransferEqual, UmiCommodityTransportRouteTransferTails, UmiCommodityTransportRouteTransferMalformed)

int main(void)
{
    UmiCommodityTransportRoute value;
    CHECK(umi_commodity_transport_route_init(&value, "ROUTE-1", "LOC-A", "LOC-B", "VESSEL") == UMI_STATUS_OK);
    CHECK(umi_commodity_transport_route_valid(&value));
    if (UmiCommodityTransportRouteTransferCases(&value) != 0) return 1;

    return 0;
}
