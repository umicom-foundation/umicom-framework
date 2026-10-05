/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/test_envelope.c
 * PURPOSE: Check fixed portable bytes and deliberately malformed but checksummed fields.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/bookmarks.h"
#include "umicom/chart/annotation.h"
#include "umicom/sdk_runtime/abi_requirement.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define REQUIRE(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; \
} } while (0)

/* Fixed bytes catch accidental byte-order or field-order changes that a
 * round-trip test would miss if both codec directions changed together. */
static const unsigned char bookmark_bytes[] = {
    0x55, 0x4d, 0x49, 0x56, 0x41, 0x4c, 0x55, 0x45, 0x6b, 0xfb, 0xec, 0x82,
    0x08, 0x1f, 0x2b, 0x7f, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x78, 0x86, 0x14, 0x45, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x66, 0x69, 0x72, 0x73, 0x74, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x68, 0x74, 0x74, 0x70, 0x73, 0x3a, 0x2f, 0x2f, 0x65, 0x78, 0x61,
    0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x69, 0x6e, 0x76, 0x61, 0x6c, 0x69, 0x64,
    0x2f, 0x6e, 0x6f, 0x74, 0x65, 0x73, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x63, 0x61, 0x66, 0xc3, 0xa9, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x77, 0x6f, 0x72, 0x6b, 0x07, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0xfb, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff,
};
/* Deliberately repair the checksum after corrupting a field so the test reaches
 * typed bounds/domain validation instead of stopping at the outer checksum. */
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
    for (size_t index = 0U; index < 4U; ++index) bytes[24U + index] = (unsigned char)(crc >> (index * 8U));
}
static int Golden(void)
{
    UmiBookmarkSnapshot value = {0}, restored;
    value.struct_size = (uint32_t)sizeof(value); value.api_version = 1U;
    memcpy(value.id, "first", sizeof("first"));
    memcpy(value.uri, "https://example.invalid/notes", sizeof("https://example.invalid/notes"));
    memcpy(value.label, "caf\xc3\xa9", sizeof("caf\xc3\xa9"));
    memcpy(value.group, "work", sizeof("work"));
    memcpy(value.icon_name, "default", sizeof("default"));
    value.order = -5; value.revision = UINT64_MAX;
    unsigned char encoded[sizeof(bookmark_bytes)];
    size_t written = 0U;
    REQUIRE(umi_platform_bookmarks_snapshot_archive_encode(&value, encoded, sizeof(encoded), &written) == UMI_STATUS_OK);
    REQUIRE(written == sizeof(bookmark_bytes) && memcmp(encoded, bookmark_bytes, written) == 0);
    REQUIRE(umi_platform_bookmarks_snapshot_archive_decode(bookmark_bytes, sizeof(bookmark_bytes), &restored) == UMI_STATUS_OK);
    REQUIRE(restored.struct_size == sizeof(restored) && restored.api_version == 1U);
    REQUIRE(strcmp(restored.id, value.id) == 0 && strcmp(restored.uri, value.uri) == 0);
    REQUIRE(strcmp(restored.label, value.label) == 0 && strcmp(restored.group, value.group) == 0);
    REQUIRE(strcmp(restored.icon_name, value.icon_name) == 0 && restored.order == -5 && restored.revision == UINT64_MAX);
    return 0;
}
static int Malformed(void)
{
    unsigned char bytes[sizeof(bookmark_bytes)];
    UmiBookmarkSnapshot value = {0};
    memcpy(value.id, "retained", sizeof("retained"));
    unsigned char before[sizeof(value)]; memcpy(before, &value, sizeof(value));
    memcpy(bytes, bookmark_bytes, sizeof(bytes)); bytes[8] ^= 1U; RepairChecksum(bytes, sizeof(bytes));
    REQUIRE(umi_platform_bookmarks_snapshot_archive_decode(bytes, sizeof(bytes), &value) == UMI_STATUS_UNAVAILABLE);
    REQUIRE(memcmp(before, &value, sizeof(value)) == 0);
    memcpy(bytes, bookmark_bytes, sizeof(bytes)); bytes[28] = 1U; RepairChecksum(bytes, sizeof(bytes));
    REQUIRE(umi_platform_bookmarks_snapshot_archive_decode(bytes, sizeof(bytes), &value) == UMI_STATUS_PARSE_ERROR);
    memcpy(bytes, bookmark_bytes, sizeof(bytes)); memset(bytes + 40U, 0xff, 8U); RepairChecksum(bytes, sizeof(bytes));
    REQUIRE(umi_platform_bookmarks_snapshot_archive_decode(bytes, sizeof(bytes), &value) == UMI_STATUS_PARSE_ERROR);
    memcpy(bytes, bookmark_bytes, sizeof(bytes)); bytes[40U + 8U] = 0U; RepairChecksum(bytes, sizeof(bytes));
    REQUIRE(umi_platform_bookmarks_snapshot_archive_decode(bytes, sizeof(bytes), &value) == UMI_STATUS_PARSE_ERROR);
    memcpy(bytes, bookmark_bytes, sizeof(bytes)); memset(bytes + 130U, 0xff, 8U); bytes[130U + 7U] = 0x7f;
    RepairChecksum(bytes, sizeof(bytes));
    REQUIRE(umi_platform_bookmarks_snapshot_archive_decode(bytes, sizeof(bytes), &value) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(memcmp(before, &value, sizeof(value)) == 0);
    UmiValueArchiveInfo info = {37U, 41U};
    unsigned char old_info[sizeof(info)]; memcpy(old_info, &info, sizeof(info));
    REQUIRE(umi_value_archive_inspect(NULL, 0U, &info) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_value_archive_inspect(bytes, 0U, &info) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(umi_value_archive_inspect(bytes, UMI_VALUE_ARCHIVE_BYTE_LIMIT + 1U, &info) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(memcmp(old_info, &info, sizeof(info)) == 0);
    return 0;
}
static int Numeric(void)
{
    UmiChartAnnotationSnapshot chart = {0}, output = {0};
    memcpy(chart.id, "numeric", sizeof("numeric"));
    chart.struct_size = (uint32_t)sizeof(chart);
    chart.value1 = -0.0; chart.value2 = 123.125;
    chart.time1 = INT64_MIN; chart.time2 = INT64_MAX;
    unsigned char bytes[4096], before[sizeof(output)]; size_t size = 0U;
    REQUIRE(umi_chart_annotation_snapshot_archive_encode(&chart, bytes, sizeof(bytes), &size) == UMI_STATUS_OK);
    REQUIRE(umi_chart_annotation_snapshot_archive_decode(bytes, size, &output) == UMI_STATUS_OK);
    REQUIRE(signbit(output.value1) && output.value2 == 123.125 && output.time1 == INT64_MIN && output.time2 == INT64_MAX);
    chart.value1 = NAN; size_t required = 99U;
    REQUIRE(umi_chart_annotation_snapshot_archive_encode(&chart, bytes, sizeof(bytes), &required) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(required == 99U);
    /* Three text fields follow api_version; the two time fields then precede
     * the first floating value. This offset derives from this fixture's text. */
    size_t number = 32U + 8U + 8U + strlen(chart.id) + 8U + 8U + 16U;
    memset(bytes + number, 0, 8U); bytes[number + 6U] = 0xf8; bytes[number + 7U] = 0x7f;
    RepairChecksum(bytes, size); memcpy(before, &output, sizeof(output));
    REQUIRE(umi_chart_annotation_snapshot_archive_decode(bytes, size, &output) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(memcmp(before, &output, sizeof(output)) == 0);
    UmiSdkRuntimeAbiRequirement requirement, restored;
    umi_sdk_runtime_abi_requirement_init(&requirement, "numeric");
    requirement.minimum_abi = 0U; requirement.maximum_abi = UINT64_MAX;
    REQUIRE(umi_sdk_runtime_abi_requirement_archive_encode(&requirement, bytes, sizeof(bytes), &size) == UMI_STATUS_OK);
    REQUIRE(umi_sdk_runtime_abi_requirement_archive_decode(bytes, size, &restored) == UMI_STATUS_OK);
    REQUIRE(restored.maximum_abi == UINT64_MAX && restored.enabled);
    /* enabled is the final field: boolean wire values must be exactly 0 or 1. */
    bytes[size - 8U] = 2U; RepairChecksum(bytes, size);
    unsigned char retained[sizeof(restored)]; memcpy(retained, &restored, sizeof(restored));
    REQUIRE(umi_sdk_runtime_abi_requirement_archive_decode(bytes, size, &restored) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(memcmp(retained, &restored, sizeof(restored)) == 0);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "golden") == 0) return Golden();
    if (strcmp(argv[1], "malformed") == 0) return Malformed();
    if (strcmp(argv[1], "numeric") == 0) return Numeric();
    return 2;
}
