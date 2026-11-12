/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/test_comparison.c
 *
 * PURPOSE:
 *   Verify comparison refuses aliasing and faulty codec outcomes without publishing a misleading observation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../../src/base/value_compare_internal.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct ComparisonProbe {
    size_t size;
    UmiStatus measure_status;
    UmiStatus write_status;
    unsigned char fill;
    int short_write;
} ComparisonProbe;

/* Injection belongs to the test codec, not the production comparator. These
 * cases check how callers observe a failing owner after scratch was touched. */
static UmiStatus ProbeEncode(const ComparisonProbe *value, void *bytes,
    size_t capacity, size_t *out_size)
{
    if (bytes == NULL) {
        if (value->measure_status != UMI_STATUS_OK) return value->measure_status;
        *out_size = value->size;
        return UMI_STATUS_OK;
    }
    if (capacity < value->size) return UMI_STATUS_CAPACITY_EXCEEDED;
    memset(bytes, value->fill, value->size);
    if (value->write_status != UMI_STATUS_OK) return value->write_status;
    *out_size = value->short_write ? value->size - 1U : value->size;
    return UMI_STATUS_OK;
}
UmiStatus ProbeCompare(const ComparisonProbe *left, const ComparisonProbe *right,
    void *workspace, size_t capacity, UmiValueArchiveComparison *out_comparison);

/* Keep accidental allocations out of the generic implementation. Allocations
 * used to construct test storage are outside this guarded instantiation. */
#define malloc UMICOM_COMPARISON_MUST_NOT_MALLOC
#define calloc UMICOM_COMPARISON_MUST_NOT_CALLOC
#define realloc UMICOM_COMPARISON_MUST_NOT_REALLOC
#define free UMICOM_COMPARISON_MUST_NOT_FREE
UMI_DEFINE_VALUE_COMPARE(ProbeCompare, ComparisonProbe, ProbeEncode)
#undef free
#undef realloc
#undef calloc
#undef malloc

#define REQUIRE(e) do { if (!(e)) {     (void)fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #e); return 1; } } while (0)

static int FailureCases(void)
{
    ComparisonProbe left = {32U, UMI_STATUS_OK, UMI_STATUS_OK, 0x21U, 0};
    ComparisonProbe right = left;
    unsigned char scratch[65], before_scratch[65];
    UmiValueArchiveComparison result, before;
    memset(&result, 0x5a, sizeof(result)); memcpy(&before, &result, sizeof(before));
    memset(scratch, 0xa5, sizeof(scratch)); memcpy(before_scratch, scratch, sizeof(scratch));

    left.measure_status = UMI_STATUS_UNAVAILABLE;
    REQUIRE(ProbeCompare(&left, &right, scratch, 64U, &result) == UMI_STATUS_UNAVAILABLE);
    REQUIRE(memcmp(&result, &before, sizeof(result)) == 0);
    REQUIRE(memcmp(scratch, before_scratch, sizeof(scratch)) == 0);
    left.measure_status = UMI_STATUS_OK;
    right.measure_status = UMI_STATUS_PARSE_ERROR;
    REQUIRE(ProbeCompare(&left, &right, scratch, 64U, &result) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(memcmp(&result, &before, sizeof(result)) == 0);
    REQUIRE(memcmp(scratch, before_scratch, sizeof(scratch)) == 0);
    right.measure_status = UMI_STATUS_OK;

    const size_t invalid_sizes[] = {0U, UMI_VALUE_ARCHIVE_HEADER_SIZE - 1U,
        UMI_VALUE_ARCHIVE_BYTE_LIMIT + 1U, SIZE_MAX};
    for (size_t i = 0U; i < sizeof(invalid_sizes) / sizeof(invalid_sizes[0]); ++i) {
        right.size = invalid_sizes[i];
        REQUIRE(ProbeCompare(&left, &right, scratch, 64U, &result) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&result, &before, sizeof(result)) == 0);
        REQUIRE(memcmp(scratch, before_scratch, sizeof(scratch)) == 0);
    }
    right.size = 32U;
    left.write_status = UMI_STATUS_IO_ERROR;
    REQUIRE(ProbeCompare(&left, &right, scratch, 64U, &result) == UMI_STATUS_IO_ERROR);
    REQUIRE(memcmp(&result, &before, sizeof(result)) == 0 && scratch[64] == 0xa5U);
    left.write_status = UMI_STATUS_OK;
    right.write_status = UMI_STATUS_CANCELLED;
    REQUIRE(ProbeCompare(&left, &right, scratch, 64U, &result) == UMI_STATUS_CANCELLED);
    REQUIRE(memcmp(&result, &before, sizeof(result)) == 0 && scratch[64] == 0xa5U);
    right.write_status = UMI_STATUS_OK;
    left.short_write = 1;
    REQUIRE(ProbeCompare(&left, &right, scratch, 64U, &result) == UMI_STATUS_INVALID_STATE);
    REQUIRE(memcmp(&result, &before, sizeof(result)) == 0);
    left.short_write = 0; right.short_write = 1;
    REQUIRE(ProbeCompare(&left, &right, scratch, 64U, &result) == UMI_STATUS_INVALID_STATE);
    REQUIRE(memcmp(&result, &before, sizeof(result)) == 0);
    right.short_write = 0;
    REQUIRE(ProbeCompare(&left, &right, scratch, 64U, &result) == UMI_STATUS_OK);
    REQUIRE(result.relation == UMI_VALUE_ARCHIVE_EQUAL);
    right.fill = 0x22U;
    REQUIRE(ProbeCompare(&left, &right, scratch, 64U, &result) == UMI_STATUS_OK);
    REQUIRE(result.relation == UMI_VALUE_ARCHIVE_DIFFERENT);
    right.fill = left.fill; right.size = 33U;
    REQUIRE(ProbeCompare(&left, &right, NULL, 0U, &result) == UMI_STATUS_OK);
    REQUIRE(result.required_workspace == 65U && result.relation == UMI_VALUE_ARCHIVE_NOT_COMPARED);
    return 0;
}

static int OverlapCases(void)
{
    ComparisonProbe left = {32U, UMI_STATUS_OK, UMI_STATUS_OK, 0x21U, 0};
    ComparisonProbe right = left, saved_left;
    memcpy(&saved_left, &left, sizeof(saved_left));
    /* The byte span begins inside a valid input, so admission must reject it
     * before the encoder can overwrite the record. No misaligned struct
     * pointer or fabricated address is needed to exercise partial overlap. */
    UmiValueArchiveComparison result = {0}, before = {0};
    memcpy(&before, &result, sizeof(before));
    REQUIRE(ProbeCompare(&left, &right, (unsigned char *)&left + 1U,
        sizeof(left) - 1U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(memcmp(&left, &saved_left, sizeof(left)) == 0);
    REQUIRE(memcmp(&result, &before, sizeof(result)) == 0);
    REQUIRE(ProbeCompare(&left, &right, (unsigned char *)&result + 1U,
        sizeof(result) - 1U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(memcmp(&result, &before, sizeof(result)) == 0);

    unsigned char scratch[65];
    memset(scratch, 0xa5, sizeof(scratch));
    REQUIRE(ProbeCompare(&left, &right, scratch + 1U, 64U, &result) == UMI_STATUS_OK);
    REQUIRE(result.relation == UMI_VALUE_ARCHIVE_EQUAL && scratch[0] == 0xa5U);
    REQUIRE(ProbeCompare(&left, &right, scratch, 0U, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(result.relation == UMI_VALUE_ARCHIVE_NOT_COMPARED && result.required_workspace == 64U);
    return 0;
}
int main(void)
{
    if (FailureCases() != 0 || OverlapCases() != 0) return 1;
    return 0;
}
