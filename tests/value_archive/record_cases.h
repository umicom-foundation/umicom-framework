/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/record_cases.h
 * PURPOSE: Check portable bytes and failure preservation through each public record type.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_VALUE_ARCHIVE_RECORD_CASES_H
#define UMICOM_TEST_VALUE_ARCHIVE_RECORD_CASES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARCHIVE_CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; \
} } while (0)

#ifdef ARCHIVE_CONDITIONAL_DECODE
#include "conditional_cases.h"

/* This fixture damages the public identity without reading outside its array. */
static void ArchiveConditionalMalformed(ARCHIVE_TYPE *value)
{
    memset(value->id, 'x', sizeof(value->id));
}
UMI_TEST_CONDITIONAL_ARCHIVE(ArchiveConditionalCases, ARCHIVE_TYPE,
    ARCHIVE_ENCODE, ARCHIVE_CONDITIONAL_DECODE, ARCHIVE_EQUAL,
    ArchiveFillUnusedText, ArchiveConditionalMalformed)

/* A changed revision is part of encoded evidence. It must refuse a delayed
 * import, then allow the same bytes when the caller reviews the newer state. */
static int ArchiveConditionalRevisionCases(const ARCHIVE_TYPE *sample)
{
    ARCHIVE_TYPE *value = malloc(sizeof(*value)), *expected = malloc(sizeof(*expected));
    unsigned char *before = malloc(sizeof(*value)), *bytes = NULL;
    size_t size = 0U, written = 0U;
    int result = 1;
    if (value == NULL || expected == NULL || before == NULL) goto construction_cleanup;
    memcpy(value, sample, sizeof(*value));
    memcpy(expected, sample, sizeof(*expected));
    UMI_CONSTRUCTION_REQUIRE(sample->revision < UINT64_MAX);
    ++value->revision;
    memcpy(before, value, sizeof(*value));
    UMI_CONSTRUCTION_REQUIRE(ARCHIVE_ENCODE(sample, NULL, 0U, &size) == UMI_STATUS_OK);
    bytes = malloc(size);
    if (bytes == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(ARCHIVE_ENCODE(sample, bytes, size, &written) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(ARCHIVE_CONDITIONAL_DECODE(bytes, size, expected, value) == UMI_STATUS_INVALID_STATE);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
    memcpy(expected, value, sizeof(*expected));
    UMI_CONSTRUCTION_REQUIRE(ARCHIVE_CONDITIONAL_DECODE(bytes, size, expected, value) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(ARCHIVE_EQUAL(sample, value));
    result = 0;
construction_cleanup:
    free(bytes); free(before); free(expected); free(value);
    return result;
}
#endif

/* Expected values come from the domain's member comparator, not a second
 * decoder. Byte comparisons are used only for deterministic output or for
 * the same destination object after an operation promises not to touch it. */
static int ArchiveRecordCases(void)
{
    ARCHIVE_TYPE value = ArchiveSample(), output = ArchiveSample();
#ifdef ARCHIVE_CONDITIONAL_DECODE
    if (ArchiveConditionalCases(&value) != 0 ||
        ArchiveConditionalRevisionCases(&value) != 0) return 1;
#endif
    size_t size = 0U, written = 99U;
    unsigned char original[sizeof(value)], before[sizeof(output)];
    memcpy(original, &value, sizeof(value));
    memcpy(before, &output, sizeof(output));
    ARCHIVE_CHECK(ARCHIVE_ENCODE(&value, NULL, 0U, &size) == UMI_STATUS_OK);
    ARCHIVE_CHECK(size > UMI_VALUE_ARCHIVE_HEADER_SIZE && size < UMI_VALUE_ARCHIVE_BYTE_LIMIT);
    unsigned char *bytes = malloc(size + 1U), *other = malloc(size + 1U);
    if (bytes == NULL || other == NULL) { free(bytes); free(other); return 1; }
    memset(bytes, 0xa5, size + 1U);
    memcpy(other, bytes, size + 1U);
    ARCHIVE_CHECK(ARCHIVE_ENCODE(&value, bytes, size - 1U, &written) == UMI_STATUS_CAPACITY_EXCEEDED);
    ARCHIVE_CHECK(written == size && memcmp(bytes, other, size + 1U) == 0);
    written = 77U;
    ARCHIVE_CHECK(ARCHIVE_ENCODE(NULL, bytes, size, &written) == UMI_STATUS_INVALID_ARGUMENT);
    ARCHIVE_CHECK(written == 77U && memcmp(bytes, other, size + 1U) == 0);
    ARCHIVE_CHECK(ARCHIVE_ENCODE(&value, NULL, 1U, &written) == UMI_STATUS_INVALID_ARGUMENT);
    ARCHIVE_CHECK(ARCHIVE_ENCODE(&value, bytes, size, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    ARCHIVE_CHECK(written == 77U && memcmp(bytes, other, size + 1U) == 0);
    ARCHIVE_CHECK(ARCHIVE_ENCODE(&value, bytes, size, &written) == UMI_STATUS_OK);
    ARCHIVE_CHECK(written == size && bytes[size] == 0xa5 && memcmp(bytes, "UMIVALUE", 8U) == 0);
    UmiValueArchiveInfo info;
    ARCHIVE_CHECK(umi_value_archive_inspect(bytes, size, &info) == UMI_STATUS_OK);
    ARCHIVE_CHECK(info.payload_size == size - UMI_VALUE_ARCHIVE_HEADER_SIZE);
    ARCHIVE_CHECK(ARCHIVE_DECODE(bytes, size, &output) == UMI_STATUS_OK);
    ARCHIVE_CHECK(ARCHIVE_EQUAL(&value, &output));
    ARCHIVE_CHECK(ARCHIVE_ENCODE(&output, other, size, &written) == UMI_STATUS_OK);
    ARCHIVE_CHECK(written == size && memcmp(bytes, other, size) == 0);
    ARCHIVE_CHECK(memcmp(original, &value, sizeof(value)) == 0);
    /* Secret or obsolete bytes after a text terminator must never be saved. */
    ArchiveFillUnusedText(&value);
    ARCHIVE_CHECK(ARCHIVE_ENCODE(&value, other, size, &written) == UMI_STATUS_OK);
    ARCHIVE_CHECK(written == size && memcmp(bytes, other, size) == 0);
    memcpy(before, &output, sizeof(output));
    const size_t truncated[] = {0U, 7U, UMI_VALUE_ARCHIVE_HEADER_SIZE - 1U,
        UMI_VALUE_ARCHIVE_HEADER_SIZE, size - 1U};
    for (size_t index = 0U; index < sizeof(truncated) / sizeof(truncated[0]); ++index) {
        ARCHIVE_CHECK(ARCHIVE_DECODE(bytes, truncated[index], &output) != UMI_STATUS_OK);
        ARCHIVE_CHECK(memcmp(before, &output, sizeof(output)) == 0);
    }
    bytes[size - 1U] ^= 1U;
    ARCHIVE_CHECK(ARCHIVE_DECODE(bytes, size, &output) == UMI_STATUS_PARSE_ERROR);
    ARCHIVE_CHECK(memcmp(before, &output, sizeof(output)) == 0);
    bytes[size - 1U] ^= 1U;
    ARCHIVE_CHECK(ARCHIVE_DECODE(bytes, size + 1U, &output) != UMI_STATUS_OK);
    ARCHIVE_CHECK(ARCHIVE_DECODE(NULL, size, &output) == UMI_STATUS_INVALID_ARGUMENT);
    ARCHIVE_CHECK(ARCHIVE_DECODE(bytes, size, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    ARCHIVE_CHECK(memcmp(before, &output, sizeof(output)) == 0);
    /* A malformed public identity is rejected before output size or bytes move. */
    memset(value.id, 'x', sizeof(value.id));
    written = 77U;
    memcpy(other, bytes, size);
    ARCHIVE_CHECK(ARCHIVE_ENCODE(&value, bytes, size, &written) != UMI_STATUS_OK);
    ARCHIVE_CHECK(written == 77U && memcmp(bytes, other, size) == 0);
    free(other);
    free(bytes);
    return 0;
}
#endif
