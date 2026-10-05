/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/test_collection.c
 * PURPOSE: Reject late corrupt rows and duplicate identities without partial restoration.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/bookmarks.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; \
} } while (0)
static uint64_t ReadInteger(const unsigned char *bytes)
{
    uint64_t result = 0U;
    for (unsigned index = 0U; index < 8U; ++index) result |= (uint64_t)bytes[index] << (index * 8U);
    return result;
}
static void RepairChecksum(unsigned char *bytes, size_t count)
{
    memset(bytes + 24U, 0, 4U);
    uint32_t crc = UINT32_MAX;
    for (size_t index = 0U; index < count; ++index) {
        crc ^= bytes[index];
        for (unsigned bit = 0U; bit < 8U; ++bit)
            crc = (crc & 1U) != 0U ? (crc >> 1U) ^ UINT32_C(0xedb88320) : crc >> 1U;
    }
    crc = ~crc;
    for (unsigned index = 0U; index < 4U; ++index) bytes[24U + index] = (unsigned char)(crc >> (index * 8U));
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiBookmarkRegistry *registry = NULL;
    REQUIRE(umi_platform_bookmarks_registry_create(&registry) == UMI_STATUS_OK);
    UmiBookmarkSnapshot value = {0};
    memcpy(value.id, "alpha", sizeof("alpha"));
    REQUIRE(umi_platform_bookmarks_registry_upsert(registry, &value) == UMI_STATUS_OK);
    memcpy(value.id, "bravo", sizeof("bravo"));
    REQUIRE(umi_platform_bookmarks_registry_upsert(registry, &value) == UMI_STATUS_OK);
    uint64_t revision = umi_platform_bookmarks_registry_revision(registry);
    size_t size = 0U;
    REQUIRE(umi_platform_bookmarks_registry_archive_encode(registry, revision, NULL, 0U, &size) == UMI_STATUS_OK);
    unsigned char *bytes = malloc(size);
    REQUIRE(bytes != NULL);
    REQUIRE(umi_platform_bookmarks_registry_archive_encode(registry, revision, bytes, size, &size) == UMI_STATUS_OK);
    /* The fixture edits a valid second row, leaving the first row acceptable.
     * Restoration must still leave the entire old collection untouched. */
    size_t second_size_position = 40U + 8U + (size_t)ReadInteger(bytes + 40U);
    size_t second_size = (size_t)ReadInteger(bytes + second_size_position);
    unsigned char *second = bytes + second_size_position + 8U;
    UmiStatus expected;
    size_t rejected = SIZE_MAX;
    if (strcmp(argv[1], "duplicate") == 0) {
        /* Row payload starts with api_version and then id length. */
        memcpy(second + 32U + 8U + 8U, "alpha", 5U);
        RepairChecksum(second, second_size);
        expected = UMI_STATUS_ALREADY_EXISTS; rejected = 1U;
    } else if (strcmp(argv[1], "late-schema") == 0) {
        second[8] ^= 1U;
        RepairChecksum(second, second_size);
        expected = UMI_STATUS_UNAVAILABLE; rejected = 1U;
    } else if (strcmp(argv[1], "late-text") == 0) {
        second[32U + 8U + 8U] = 0U;
        RepairChecksum(second, second_size);
        expected = UMI_STATUS_PARSE_ERROR; rejected = 1U;
    } else if (strcmp(argv[1], "count") == 0) {
        memset(bytes + 32U, 0xff, 8U);
        expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(argv[1], "row-length") == 0) {
        memset(bytes + second_size_position, 0xff, 8U);
        expected = UMI_STATUS_PARSE_ERROR; rejected = 1U;
    } else { free(bytes); umi_platform_bookmarks_registry_destroy(registry); return 2; }
    RepairChecksum(bytes, size);
    UmiSnapshotBatchResult result;
    REQUIRE(umi_platform_bookmarks_registry_archive_restore(registry, revision, bytes, size, &result) == expected);
    REQUIRE(result.applied == 0U && result.rejected_index == rejected);
    REQUIRE(umi_platform_bookmarks_registry_revision(registry) == revision && umi_platform_bookmarks_registry_count(registry) == 2U);
    REQUIRE(umi_platform_bookmarks_registry_at(registry, 0U, &value) == UMI_STATUS_OK && strcmp(value.id, "alpha") == 0);
    REQUIRE(umi_platform_bookmarks_registry_at(registry, 1U, &value) == UMI_STATUS_OK && strcmp(value.id, "bravo") == 0);
    free(bytes);
    umi_platform_bookmarks_registry_destroy(registry);
    return 0;
}
