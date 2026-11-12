/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gpt_inspection/test_gpt.c
 *
 * PURPOSE:
 *   Verify successful GPT observations and precise refusal boundaries with synthetic memory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); \
    failed = 1; goto cleanup; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    static const char *const cases[] = { "valid", "four-k", "small-slot-count", "wide-entries", "widest-entries", "largest-profile", "empty", "unused-fields", "adjacent", "one-sector", "attributes", "null-reader", "null-output", "zero-bytes", "short", "unaligned", "bad-block", "signature", "revision", "header-small", "header-large", "header-crc", "header-reserved", "header-padding", "header-location", "alternate", "disk-guid", "count-zero", "count-large", "stride-small", "stride-nonpower", "stride-large", "primary-reservation", "backup-reservation", "primary-table", "backup-table", "first-last", "last-backup", "backup-signature", "backup-crc", "backup-location", "backup-alternate", "backup-guid", "backup-first", "backup-last", "backup-count", "backup-width", "backup-table-crc", "backup-size", "table-crc", "table-mismatch", "missing-partition-guid", "partition-before", "partition-after", "partition-inverted", "partition-overlap", "duplicate-guid", "entry-reserved", "unused-reserved", "reserved-attributes", "read-primary", "read-backup", "read-table", "read-backup-table" };
    int known = 0, failed = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0) known = 1;
    if (!known) return 2;
    GptFixture fixture = {0};
    uint32_t block = strcmp(name, "four-k") == 0 ? 4096U : 512U;
    uint32_t width = strcmp(name, "wide-entries") == 0 ||
        strcmp(name, "entry-reserved") == 0 || strcmp(name, "unused-reserved") == 0 ? 256U :
        strcmp(name, "widest-entries") == 0 || strcmp(name, "largest-profile") == 0 ? 512U : 128U;
    uint32_t count = strcmp(name, "small-slot-count") == 0 ||
        strcmp(name, "primary-reservation") == 0 || strcmp(name, "backup-reservation") == 0 ? 4U :
        strcmp(name, "largest-profile") == 0 ? 256U : 128U;
    CHECK(FixtureCreate(&fixture, block, count, width));
    CHECK(FixtureChecksum((const unsigned char *)"123456789", 9U) == UINT32_C(0xcbf43926));
    unsigned char *primary = FixtureHeader(&fixture, 0U), *backup = FixtureHeader(&fixture, 1U);
    unsigned char *entry = FixtureEntry(&fixture, 0U), *second = FixtureEntry(&fixture, 1U);
    UmiStatus expected = UMI_STATUS_PARSE_ERROR;
    uint64_t bytes = GPT_FIXTURE_BYTES;
    unsigned partitions = 1U, expectedCalls = 4U;
    UmiGptReadAt readAt = FixtureRead;
    UmiGptImageInfo output, untouched;
    memset(&output, 0xa5, sizeof(output));
    memcpy(&untouched, &output, sizeof(output));
    UmiGptImageInfo *destination = &output;
    if (!strcmp(name, "valid") || !strcmp(name, "four-k") || !strcmp(name, "small-slot-count") ||
        !strcmp(name, "wide-entries") || !strcmp(name, "widest-entries") ||
        !strcmp(name, "largest-profile")) expected = UMI_STATUS_OK;
    else if (!strcmp(name, "empty")) {
        memset(entry, 0, width); partitions = 0U; expected = UMI_STATUS_OK; FixtureTables(&fixture);
    } else if (!strcmp(name, "unused-fields")) {
        second[16U] = 99U; Fixture64(second + 32U, UINT64_MAX);
        FixtureTables(&fixture); expected = UMI_STATUS_OK;
    } else if (!strcmp(name, "adjacent") || !strcmp(name, "partition-overlap") || !strcmp(name, "duplicate-guid")) {
        memcpy(second, entry, width); second[16U] = !strcmp(name, "duplicate-guid") ? 2U : 3U;
        Fixture64(second + 32U, fixture.first + (!strcmp(name, "partition-overlap") ? 10U : 11U));
        Fixture64(second + 40U, fixture.first + 20U); FixtureTables(&fixture);
        if (!strcmp(name, "adjacent")) { expected = UMI_STATUS_OK; partitions = 2U; }
    } else if (!strcmp(name, "one-sector")) {
        Fixture64(entry + 40U, fixture.first); FixtureTables(&fixture); expected = UMI_STATUS_OK;
    } else if (!strcmp(name, "attributes")) {
        Fixture64(entry + 48U, UINT64_C(0xffff000000000007));
        FixtureTables(&fixture); expected = UMI_STATUS_OK;
    } else if (!strcmp(name, "null-reader")) {
        readAt = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; expectedCalls = 0U;
    } else if (!strcmp(name, "null-output")) {
        destination = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; expectedCalls = 0U;
    } else if (!strcmp(name, "zero-bytes")) {
        bytes = 0U; expected = UMI_STATUS_INVALID_ARGUMENT; expectedCalls = 0U;
    } else if (!strcmp(name, "short")) {
        bytes = 2U * block; expectedCalls = 0U;
    } else if (!strcmp(name, "unaligned")) {
        --bytes; expected = UMI_STATUS_INVALID_ARGUMENT; expectedCalls = 0U;
    } else if (!strcmp(name, "bad-block")) {
        block = 1024U; expected = UMI_STATUS_INVALID_ARGUMENT; expectedCalls = 0U;
    } else if (!strcmp(name, "signature")) primary[0] = 0U;
    else if (!strcmp(name, "revision")) {
        Fixture32(primary + 8U, UINT32_C(0x20000)); expected = UMI_STATUS_UNAVAILABLE;
    } else if (!strcmp(name, "header-small")) Fixture32(primary + 12U, 91U);
    else if (!strcmp(name, "header-large")) Fixture32(primary + 12U, block + 1U);
    else if (!strcmp(name, "header-crc")) primary[16U] ^= 1U;
    else if (!strcmp(name, "header-reserved")) { primary[20U] = 1U; FixtureHeaderChecksum(&fixture, 0U, 92U); }
    else if (!strcmp(name, "header-padding")) primary[block - 1U] = 1U;
    else if (!strcmp(name, "header-location")) { Fixture64(primary + 24U, 2U); FixtureHeaderChecksum(&fixture, 0U, 92U); }
    else if (!strcmp(name, "alternate")) { Fixture64(primary + 32U, fixture.sectors - 2U); FixtureHeaderChecksum(&fixture, 0U, 92U); }
    else if (!strcmp(name, "disk-guid")) { memset(primary + 56U, 0, 16U); FixtureHeaderChecksum(&fixture, 0U, 92U); }
    else if (!strcmp(name, "count-zero") || !strcmp(name, "count-large")) {
        Fixture32(primary + 80U, !strcmp(name, "count-zero") ? 0U : UINT32_MAX);
        FixtureHeaderChecksum(&fixture, 0U, 92U);
        if (!strcmp(name, "count-large")) expected = UMI_STATUS_UNAVAILABLE;
    } else if (!strcmp(name, "stride-small") || !strcmp(name, "stride-nonpower") || !strcmp(name, "stride-large")) {
        Fixture32(primary + 84U, !strcmp(name, "stride-small") ? 64U :
            !strcmp(name, "stride-nonpower") ? 384U : UINT32_C(0x80000000));
        FixtureHeaderChecksum(&fixture, 0U, 92U);
        if (!strcmp(name, "stride-large")) expected = UMI_STATUS_UNAVAILABLE;
    } else if (!strcmp(name, "primary-reservation")) {
        Fixture64(primary + 40U, fixture.first - 1U); FixtureHeaderChecksum(&fixture, 0U, 92U);
    } else if (!strcmp(name, "backup-reservation")) {
        Fixture64(backup + 72U, fixture.backupTable + 1U); FixtureHeaderChecksum(&fixture, 1U, 92U);
    } else if (!strcmp(name, "primary-table")) {
        Fixture64(primary + 72U, UINT64_MAX); FixtureHeaderChecksum(&fixture, 0U, 92U);
    } else if (!strcmp(name, "backup-table")) {
        Fixture64(backup + 72U, fixture.last); FixtureHeaderChecksum(&fixture, 1U, 92U);
    } else if (!strcmp(name, "first-last")) {
        Fixture64(primary + 40U, fixture.last + 1U); FixtureHeaderChecksum(&fixture, 0U, 92U);
    } else if (!strcmp(name, "last-backup")) {
        Fixture64(primary + 48U, fixture.sectors - 1U); FixtureHeaderChecksum(&fixture, 0U, 92U);
    } else if (!strcmp(name, "backup-signature")) backup[0] = 0U;
    else if (!strcmp(name, "backup-crc")) backup[16U] ^= 1U;
    else if (!strcmp(name, "backup-location")) { Fixture64(backup + 24U, 1U); FixtureHeaderChecksum(&fixture, 1U, 92U); }
    else if (!strcmp(name, "backup-alternate")) { Fixture64(backup + 32U, 2U); FixtureHeaderChecksum(&fixture, 1U, 92U); }
    else if (!strcmp(name, "backup-guid")) { backup[56U] = 2U; FixtureHeaderChecksum(&fixture, 1U, 92U); }
    else if (!strcmp(name, "backup-first")) { Fixture64(backup + 40U, fixture.first + 1U); FixtureHeaderChecksum(&fixture, 1U, 92U); }
    else if (!strcmp(name, "backup-last")) { Fixture64(backup + 48U, fixture.last - 1U); FixtureHeaderChecksum(&fixture, 1U, 92U); }
    else if (!strcmp(name, "backup-count")) { Fixture32(backup + 80U, count - 1U); FixtureHeaderChecksum(&fixture, 1U, 92U); }
    else if (!strcmp(name, "backup-width")) { Fixture32(backup + 84U, width * 2U); FixtureHeaderChecksum(&fixture, 1U, 92U); }
    else if (!strcmp(name, "backup-table-crc")) { backup[88U] ^= 1U; FixtureHeaderChecksum(&fixture, 1U, 92U); }
    else if (!strcmp(name, "backup-size")) { Fixture32(backup + 12U, 96U); FixtureHeaderChecksum(&fixture, 1U, 96U); }
    else if (!strcmp(name, "table-crc")) entry[16U] ^= 1U;
    else if (!strcmp(name, "table-mismatch")) fixture.data[fixture.backupTable * block + 16U] ^= 1U;
    else if (!strcmp(name, "missing-partition-guid")) { memset(entry + 16U, 0, 16U); FixtureTables(&fixture); }
    else if (!strcmp(name, "partition-before")) { Fixture64(entry + 32U, fixture.first - 1U); FixtureTables(&fixture); }
    else if (!strcmp(name, "partition-after")) { Fixture64(entry + 40U, fixture.last + 1U); FixtureTables(&fixture); }
    else if (!strcmp(name, "partition-inverted")) { Fixture64(entry + 32U, fixture.first + 11U); FixtureTables(&fixture); }
    else if (!strcmp(name, "entry-reserved")) { entry[128U] = 1U; FixtureTables(&fixture); }
    else if (!strcmp(name, "unused-reserved")) { second[128U] = 1U; FixtureTables(&fixture); }
    else if (!strcmp(name, "reserved-attributes")) { Fixture64(entry + 48U, 8U); FixtureTables(&fixture); }
    else {
        fixture.failAt = !strcmp(name, "read-primary") ? 1U : !strcmp(name, "read-backup") ? 2U :
            !strcmp(name, "read-table") ? 3U : 4U;
        expected = UMI_STATUS_IO_ERROR; expectedCalls = fixture.failAt;
    }
    UmiStatus actual = UmiGptImageInspect(readAt, &fixture, bytes, block, destination);
    CHECK(actual == expected && fixture.outside == 0U && fixture.calls <= 4U);
    if (actual == UMI_STATUS_OK) {
        CHECK(output.partitions == partitions && output.logicalBlockBytes == block &&
            output.firstUsableLba == fixture.first && output.lastUsableLba == fixture.last);
        CHECK(fixture.calls == 4U);
    } else {
        CHECK(memcmp(&output, &untouched, sizeof(output)) == 0);
        if (expectedCalls != 4U || expected == UMI_STATUS_IO_ERROR)
            CHECK(fixture.calls == expectedCalls);
    }
cleanup:
    free(fixture.data);
    return failed;
}
