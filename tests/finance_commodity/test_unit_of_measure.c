/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_unit_of_measure.c
 *
 * PURPOSE:
 *   Implement the test unit of measure behavior for
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

#include "umicom/finance/commodity/unit_of_measure.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/unit_of_measure.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityUnitOfMeasureTransferEqual(const UmiCommodityUnitOfMeasure *a, const UmiCommodityUnitOfMeasure *b)
{
    return strcmp(a->code, b->code) == 0 &&
        strcmp(a->dimension, b->dimension) == 0 &&
        a->numerator == b->numerator &&
        a->denominator == b->denominator &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityUnitOfMeasureTransferTails(UmiCommodityUnitOfMeasure *value)
{
    (void)value;
    {
        size_t used = strlen(value->code) + 1U;
        memset(value->code + used, 0xa5, sizeof(value->code) - used);
    }
    {
        size_t used = strlen(value->dimension) + 1U;
        memset(value->dimension + used, 0xa5, sizeof(value->dimension) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityUnitOfMeasureTransferMalformed(const UmiCommodityUnitOfMeasure *sample)
{
    (void)sample;
    {
        UmiCommodityUnitOfMeasure invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.code, 'x', sizeof(invalid.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_unit_of_measure_valid(&invalid)) ||
            umi_commodity_unit_of_measure_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated code was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityUnitOfMeasure invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.dimension, 'x', sizeof(invalid.dimension));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_unit_of_measure_valid(&invalid)) ||
            umi_commodity_unit_of_measure_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated dimension was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityUnitOfMeasureTransferCases, UmiCommodityUnitOfMeasure,
    umi_commodity_unit_of_measure_archive_encode, umi_commodity_unit_of_measure_archive_decode,
    UmiCommodityUnitOfMeasureTransferEqual, UmiCommodityUnitOfMeasureTransferTails, UmiCommodityUnitOfMeasureTransferMalformed)

int main(void)
{
    UmiCommodityUnitOfMeasure value;
    CHECK(umi_commodity_unit_of_measure_init(&value, "BBL", "VOLUME", 158987, 1000) == UMI_STATUS_OK);
    CHECK(umi_commodity_unit_of_measure_valid(&value));
    if (UmiCommodityUnitOfMeasureTransferCases(&value) != 0) return 1;

    return 0;
}
