/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_quality_measure.c
 *
 * PURPOSE:
 *   Implement the test quality measure behavior for
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

#include "umicom/finance/commodity/quality_measure.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/quality_measure.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityQualityMeasureTransferEqual(const UmiCommodityQualityMeasure *a, const UmiCommodityQualityMeasure *b)
{
    return strcmp(a->name, b->name) == 0 &&
        strcmp(a->unit_code, b->unit_code) == 0 &&
        a->minimum == b->minimum &&
        a->maximum == b->maximum &&
        a->scale == b->scale;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityQualityMeasureTransferTails(UmiCommodityQualityMeasure *value)
{
    (void)value;
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->unit_code) + 1U;
        memset(value->unit_code + used, 0xa5, sizeof(value->unit_code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityQualityMeasureTransferMalformed(const UmiCommodityQualityMeasure *sample)
{
    (void)sample;
    {
        UmiCommodityQualityMeasure invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_quality_measure_valid(&invalid)) ||
            umi_commodity_quality_measure_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityQualityMeasure invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.unit_code, 'x', sizeof(invalid.unit_code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_quality_measure_valid(&invalid)) ||
            umi_commodity_quality_measure_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated unit_code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityQualityMeasureTransferCases, UmiCommodityQualityMeasure,
    umi_commodity_quality_measure_archive_encode, umi_commodity_quality_measure_archive_decode,
    UmiCommodityQualityMeasureTransferEqual, UmiCommodityQualityMeasureTransferTails, UmiCommodityQualityMeasureTransferMalformed)

int main(void)
{
    UmiCommodityQualityMeasure value;
    CHECK(umi_commodity_quality_measure_init(&value, "sulphur", "PCT", 0, 50, 2) == UMI_STATUS_OK);
    CHECK(umi_commodity_quality_measure_valid(&value));
    if (UmiCommodityQualityMeasureTransferCases(&value) != 0) return 1;

    CHECK(value.maximum == 50);
    return 0;
}
