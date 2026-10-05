/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/transfer_cases.h
 * PURPOSE: Check saved value boundaries through the real public library functions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_VALUE_ARCHIVE_TRANSFER_CASES_H
#define UMICOM_TEST_VALUE_ARCHIVE_TRANSFER_CASES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* These checks stay active in every build configuration. Domain fixtures
 * supply an already valid object; comparisons enumerate its public fields,
 * while byte comparisons check only deterministic wire data and no-write
 * guarantees on the same object. Release builds must exercise the same calls. */
#define UMI_TRANSFER_REQUIRE(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
    result = 1; goto transfer_done; \
} } while (0)

#define UMI_TEST_VALUE_TRANSFER(Name, Type, Encode, Decode, Equal, Tails, Malformed) \
static int Name(const Type *sample) \
{ \
    Type value = *sample, output = *sample; \
    unsigned char original[sizeof(value)], before[sizeof(output)]; \
    unsigned char *bytes = NULL, *other = NULL; \
    size_t size = 0U, written = 77U; \
    int result = 0; \
    memcpy(original, &value, sizeof(value)); memcpy(before, &output, sizeof(output)); \
    UMI_TRANSFER_REQUIRE(Encode(&value, NULL, 0U, &size) == UMI_STATUS_OK); \
    UMI_TRANSFER_REQUIRE(size > UMI_VALUE_ARCHIVE_HEADER_SIZE && size <= UMI_VALUE_ARCHIVE_BYTE_LIMIT); \
    bytes = malloc(size + 1U); other = malloc(size + 1U); \
    UMI_TRANSFER_REQUIRE(bytes != NULL && other != NULL); \
    memset(bytes, 0xa5, size + 1U); memcpy(other, bytes, size + 1U); \
    UMI_TRANSFER_REQUIRE(Encode(&value, bytes, size - 1U, &written) == UMI_STATUS_CAPACITY_EXCEEDED); \
    UMI_TRANSFER_REQUIRE(written == size && memcmp(bytes, other, size + 1U) == 0); \
    written = 77U; \
    UMI_TRANSFER_REQUIRE(Encode(NULL, bytes, size, &written) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_TRANSFER_REQUIRE(written == 77U && memcmp(bytes, other, size + 1U) == 0); \
    UMI_TRANSFER_REQUIRE(Encode(&value, NULL, 1U, &written) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_TRANSFER_REQUIRE(Encode(&value, bytes, size, NULL) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_TRANSFER_REQUIRE(Encode(&value, bytes, size, &written) == UMI_STATUS_OK); \
    UMI_TRANSFER_REQUIRE(written == size && bytes[size] == 0xa5); \
    UMI_TRANSFER_REQUIRE(Decode(bytes, size, &output) == UMI_STATUS_OK); \
    UMI_TRANSFER_REQUIRE(Equal(&value, &output)); \
    UMI_TRANSFER_REQUIRE(Encode(&output, other, size, &written) == UMI_STATUS_OK); \
    UMI_TRANSFER_REQUIRE(written == size && memcmp(bytes, other, size) == 0); \
    UMI_TRANSFER_REQUIRE(memcmp(original, &value, sizeof(value)) == 0); \
    Tails(&value); \
    UMI_TRANSFER_REQUIRE(Encode(&value, other, size, &written) == UMI_STATUS_OK); \
    UMI_TRANSFER_REQUIRE(written == size && memcmp(bytes, other, size) == 0); \
    memcpy(before, &output, sizeof(output)); \
    const size_t short_sizes[] = {0U, 7U, UMI_VALUE_ARCHIVE_HEADER_SIZE - 1U, size - 1U}; \
    for (size_t index = 0U; index < sizeof(short_sizes) / sizeof(short_sizes[0]); ++index) { \
        UMI_TRANSFER_REQUIRE(Decode(bytes, short_sizes[index], &output) != UMI_STATUS_OK); \
        UMI_TRANSFER_REQUIRE(memcmp(before, &output, sizeof(output)) == 0); \
    } \
    bytes[size - 1U] ^= 1U; \
    UMI_TRANSFER_REQUIRE(Decode(bytes, size, &output) == UMI_STATUS_PARSE_ERROR); \
    UMI_TRANSFER_REQUIRE(memcmp(before, &output, sizeof(output)) == 0); \
    bytes[size - 1U] ^= 1U; \
    UMI_TRANSFER_REQUIRE(Decode(bytes, size + 1U, &output) != UMI_STATUS_OK); \
    UMI_TRANSFER_REQUIRE(Decode(NULL, size, &output) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_TRANSFER_REQUIRE(Decode(bytes, size, NULL) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_TRANSFER_REQUIRE(memcmp(before, &output, sizeof(output)) == 0); \
    UMI_TRANSFER_REQUIRE(Malformed(sample) == 0); \
transfer_done: \
    free(other); free(bytes); \
    return result; \
}
#endif
