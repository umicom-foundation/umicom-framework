/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_location.c
 *
 * PURPOSE:
 *   Implement the test location behavior for
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

#include "umicom/finance/commodity/location.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/location.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityLocationTransferEqual(const UmiCommodityLocation *a, const UmiCommodityLocation *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        strcmp(a->country_code, b->country_code) == 0 &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityLocationTransferTails(UmiCommodityLocation *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->country_code) + 1U;
        memset(value->country_code + used, 0xa5, sizeof(value->country_code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityLocationTransferMalformed(const UmiCommodityLocation *sample)
{
    (void)sample;
    {
        UmiCommodityLocation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_location_valid(&invalid)) ||
            umi_commodity_location_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityLocation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_location_valid(&invalid)) ||
            umi_commodity_location_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityLocation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.country_code, 'x', sizeof(invalid.country_code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_location_valid(&invalid)) ||
            umi_commodity_location_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated country_code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityLocationTransferCases, UmiCommodityLocation,
    umi_commodity_location_archive_encode, umi_commodity_location_archive_decode,
    UmiCommodityLocationTransferEqual, UmiCommodityLocationTransferTails, UmiCommodityLocationTransferMalformed)

int main(void)
{
    UmiCommodityLocation value;
    CHECK(umi_commodity_location_init(&value, "LOC-CUSHING", "Cushing", "US") == UMI_STATUS_OK);
    CHECK(umi_commodity_location_valid(&value));
    if (UmiCommodityLocationTransferCases(&value) != 0) return 1;

    return 0;
}
