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

/* Expected values come from the domain's member comparator, not a second
 * decoder. Byte comparisons are used only for deterministic output or for
 * the same destination object after an operation promises not to touch it. */
static int ArchiveRecordCases(void)
{
    ARCHIVE_TYPE value = ArchiveSample(), output = ArchiveSample();
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
