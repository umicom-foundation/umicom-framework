/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_grade_specification.c
 *
 * PURPOSE:
 *   Implement the test grade specification behavior for
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

#include "umicom/finance/commodity/grade_specification.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/grade_specification.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityGradeSpecificationTransferEqual(const UmiCommodityGradeSpecification *a, const UmiCommodityGradeSpecification *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->commodity_id.value, b->commodity_id.value) == 0 &&
        strcmp(a->grade_code, b->grade_code) == 0 &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityGradeSpecificationTransferTails(UmiCommodityGradeSpecification *value)
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
        size_t used = strlen(value->grade_code) + 1U;
        memset(value->grade_code + used, 0xa5, sizeof(value->grade_code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityGradeSpecificationTransferMalformed(const UmiCommodityGradeSpecification *sample)
{
    (void)sample;
    {
        UmiCommodityGradeSpecification invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_grade_specification_valid(&invalid)) ||
            umi_commodity_grade_specification_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityGradeSpecification invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.commodity_id.value, 'x', sizeof(invalid.commodity_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_grade_specification_valid(&invalid)) ||
            umi_commodity_grade_specification_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated commodity_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityGradeSpecification invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.grade_code, 'x', sizeof(invalid.grade_code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_grade_specification_valid(&invalid)) ||
            umi_commodity_grade_specification_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated grade_code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityGradeSpecificationTransferCases, UmiCommodityGradeSpecification,
    umi_commodity_grade_specification_archive_encode, umi_commodity_grade_specification_archive_decode,
    UmiCommodityGradeSpecificationTransferEqual, UmiCommodityGradeSpecificationTransferTails, UmiCommodityGradeSpecificationTransferMalformed)

int main(void)
{
    UmiCommodityGradeSpecification value;
    CHECK(umi_commodity_grade_specification_init(&value, "GRADE-BRENT", "CMD-BRENT", "BFOET") == UMI_STATUS_OK);
    CHECK(umi_commodity_grade_specification_valid(&value));
    if (UmiCommodityGradeSpecificationTransferCases(&value) != 0) return 1;

    CHECK(value.active);
    return 0;
}
