/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_storage_facility.c
 *
 * PURPOSE:
 *   Implement the test storage facility behavior for
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

#include "umicom/finance/commodity/storage_facility.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/storage_facility.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityStorageFacilityTransferEqual(const UmiCommodityStorageFacility *a, const UmiCommodityStorageFacility *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->location_id.value, b->location_id.value) == 0 &&
        a->capacity.units == b->capacity.units &&
        a->capacity.scale == b->capacity.scale &&
        strcmp(a->capacity.unit_code, b->capacity.unit_code) == 0 &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityStorageFacilityTransferTails(UmiCommodityStorageFacility *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->location_id.value) + 1U;
        memset(value->location_id.value + used, 0xa5, sizeof(value->location_id.value) - used);
    }
    {
        size_t used = strlen(value->capacity.unit_code) + 1U;
        memset(value->capacity.unit_code + used, 0xa5, sizeof(value->capacity.unit_code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityStorageFacilityTransferMalformed(const UmiCommodityStorageFacility *sample)
{
    (void)sample;
    {
        UmiCommodityStorageFacility invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_storage_facility_valid(&invalid)) ||
            umi_commodity_storage_facility_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityStorageFacility invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.location_id.value, 'x', sizeof(invalid.location_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_storage_facility_valid(&invalid)) ||
            umi_commodity_storage_facility_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated location_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityStorageFacility invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.capacity.unit_code, 'x', sizeof(invalid.capacity.unit_code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_storage_facility_valid(&invalid)) ||
            umi_commodity_storage_facility_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated capacity.unit_code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityStorageFacilityTransferCases, UmiCommodityStorageFacility,
    umi_commodity_storage_facility_archive_encode, umi_commodity_storage_facility_archive_decode,
    UmiCommodityStorageFacilityTransferEqual, UmiCommodityStorageFacilityTransferTails, UmiCommodityStorageFacilityTransferMalformed)

int main(void)
{
    UmiCommodityStorageFacility value;
    CHECK(umi_commodity_storage_facility_init(&value, "FAC-1", "LOC-1", 100000, 0, "BBL") == UMI_STATUS_OK);
    CHECK(umi_commodity_storage_facility_valid(&value));
    if (UmiCommodityStorageFacilityTransferCases(&value) != 0) return 1;

    CHECK(value.capacity.units == 100000);
    return 0;
}
