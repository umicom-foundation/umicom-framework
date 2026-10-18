/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/workspace_cases.h
 * PURPOSE: Verify caller-owned decoding scratch through real public codecs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_ARCHIVE_WORKSPACE_CASES_H
#define UMICOM_TEST_ARCHIVE_WORKSPACE_CASES_H
#include "umicom/base/value_archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Repair deliberately edited envelopes independently of the production helper.
 * This makes schema and truncated-field cases reach typed validation instead
 * of being rejected only because an earlier checksum no longer matches. */
static inline void UmiWorkspaceFixtureInteger(unsigned char *bytes, uint64_t value)
{
    for (size_t i = 0U; i < 8U; ++i) {
        bytes[i] = (unsigned char)(value & UINT64_C(255));
        value >>= 8U;
    }
}
static inline void UmiWorkspaceFixtureChecksum(unsigned char *bytes, size_t count)
{
    memset(bytes + 24U, 0, 4U);
    uint32_t crc = UINT32_MAX;
    for (size_t i = 0U; i < count; ++i) {
        crc ^= bytes[i];
        for (unsigned bit = 0U; bit < 8U; ++bit)
            crc = (crc & 1U) != 0U ? (crc >> 1U) ^ UINT32_C(0xedb88320) : crc >> 1U;
    }
    crc = ~crc;
    for (size_t i = 0U; i < 4U; ++i)
        bytes[24U + i] = (unsigned char)(crc >> (i * 8U));
}
#define UMI_WORKSPACE_REQUIRE(Expression) do { if (!(Expression)) { \
    fprintf(stderr, "%s:%d: workspace decode contract: %s\n", __FILE__, __LINE__, #Expression); \
    goto workspace_done; \
} } while (0)

/* The sample and field comparator belong to the domain fixture. Keep every
 * whole record on the heap here so the new path is not accidentally tested
 * with the same large automatic objects it is intended to avoid in consumers. */
#define UMI_TEST_VALUE_WORKSPACE(Name, Type, Encode, Decode, Equal) \
static int Name(const Type *sample) \
{ \
    Type *workspace = malloc(sizeof(*workspace)), *value = malloc(sizeof(*value)); \
    unsigned char *before = malloc(sizeof(*value)), *bytes = NULL, *edited = NULL; \
    size_t size = 0U, written = 0U; \
    int result = 1; \
    UMI_WORKSPACE_REQUIRE(workspace != NULL && value != NULL && before != NULL); \
    UMI_WORKSPACE_REQUIRE(Encode(sample, NULL, 0U, &size) == UMI_STATUS_OK); \
    UMI_WORKSPACE_REQUIRE(size > UMI_VALUE_ARCHIVE_HEADER_SIZE && size <= UMI_VALUE_ARCHIVE_BYTE_LIMIT); \
    bytes = malloc(size + 1U); edited = malloc(size + 1U); \
    UMI_WORKSPACE_REQUIRE(bytes != NULL && edited != NULL); \
    UMI_WORKSPACE_REQUIRE(Encode(sample, bytes, size, &written) == UMI_STATUS_OK && written == size); \
    memset(workspace, 0xa5, sizeof(*workspace)); \
    memset(value, 0x5a, sizeof(*value)); \
    UMI_WORKSPACE_REQUIRE(Decode(bytes, size, workspace, value) == UMI_STATUS_OK); \
    UMI_WORKSPACE_REQUIRE(Equal(sample, value)); \
    UMI_WORKSPACE_REQUIRE(Encode(value, edited, size, &written) == UMI_STATUS_OK); \
    UMI_WORKSPACE_REQUIRE(written == size && memcmp(bytes, edited, size) == 0); \
    memcpy(before, value, sizeof(*value)); \
    UMI_WORKSPACE_REQUIRE(Decode(NULL, size, workspace, value) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_WORKSPACE_REQUIRE(Decode(bytes, size, NULL, value) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_WORKSPACE_REQUIRE(Decode(bytes, size, workspace, NULL) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_WORKSPACE_REQUIRE(Decode(bytes, size, value, value) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_WORKSPACE_REQUIRE(Decode(workspace, 1U, workspace, value) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_WORKSPACE_REQUIRE(Decode(value, 1U, workspace, value) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_WORKSPACE_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    const size_t short_sizes[] = {0U, 7U, UMI_VALUE_ARCHIVE_HEADER_SIZE - 1U, size - 1U}; \
    for (size_t i = 0U; i < sizeof(short_sizes) / sizeof(short_sizes[0]); ++i) { \
        UMI_WORKSPACE_REQUIRE(Decode(bytes, short_sizes[i], workspace, value) != UMI_STATUS_OK); \
        UMI_WORKSPACE_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    } \
    memcpy(edited, bytes, size); edited[size - 1U] ^= 1U; \
    UMI_WORKSPACE_REQUIRE(Decode(edited, size, workspace, value) == UMI_STATUS_PARSE_ERROR); \
    UMI_WORKSPACE_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    memcpy(edited, bytes, size); edited[8U] ^= 1U; \
    UmiWorkspaceFixtureChecksum(edited, size); \
    UMI_WORKSPACE_REQUIRE(Decode(edited, size, workspace, value) == UMI_STATUS_UNAVAILABLE); \
    UMI_WORKSPACE_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    memcpy(edited, bytes, size); \
    UmiWorkspaceFixtureInteger(edited + 16U, (uint64_t)(size - 1U - UMI_VALUE_ARCHIVE_HEADER_SIZE)); \
    UmiWorkspaceFixtureChecksum(edited, size - 1U); \
    UMI_WORKSPACE_REQUIRE(Decode(edited, size - 1U, workspace, value) == UMI_STATUS_PARSE_ERROR); \
    UMI_WORKSPACE_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    UMI_WORKSPACE_REQUIRE(Decode(bytes, size, workspace, value) == UMI_STATUS_OK); \
    UMI_WORKSPACE_REQUIRE(Equal(sample, value)); \
    memcpy(before, value, sizeof(*value)); \
    memset(workspace, 0xa5, sizeof(*workspace)); \
    UMI_WORKSPACE_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    result = 0; \
workspace_done: \
    free(edited); free(bytes); free(before); free(value); free(workspace); \
    return result; \
}
#endif
