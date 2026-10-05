/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/snapshot_archive_cases.h
 * PURPOSE: Verify complete archive restoration, stale refusal and damaged-input rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_SNAPSHOT_ARCHIVE_CASES_H
#define UMICOM_TEST_SNAPSHOT_ARCHIVE_CASES_H
static int ArchiveCollection(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT value = Record("saved-first"), second = Record("saved-second"), expected[2], output;
    CHECK(CONTRACT_UPSERT(registry, &value) == UMI_STATUS_OK);
    CHECK(CONTRACT_UPSERT(registry, &second) == UMI_STATUS_OK);
    CHECK(CONTRACT_AT(registry, 0U, &expected[0]) == UMI_STATUS_OK);
    CHECK(CONTRACT_AT(registry, 1U, &expected[1]) == UMI_STATUS_OK);
    uint64_t observed = CONTRACT_REVISION(registry);
    size_t size = 0U, written = 0U;
    CHECK(CONTRACT_ARCHIVE_ENCODE(registry, observed, NULL, 0U, &size) == UMI_STATUS_OK);
    unsigned char *bytes = malloc(size), *sentinel = malloc(size);
    if (bytes == NULL || sentinel == NULL) { free(bytes); free(sentinel); return 1; }
    memset(bytes, 0xa5, size); memcpy(sentinel, bytes, size);
    CHECK(CONTRACT_ARCHIVE_ENCODE(registry, observed, bytes, size - 1U, &written) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(written == size && memcmp(sentinel, bytes, size) == 0);
    CHECK(CONTRACT_ARCHIVE_ENCODE(registry, observed, bytes, size, &written) == UMI_STATUS_OK);
    CHECK(CONTRACT_REMOVE(registry, "saved-first") == UMI_STATUS_OK);
    uint64_t current = CONTRACT_REVISION(registry);
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_ARCHIVE_RESTORE(registry, observed, bytes, size, &result) == UMI_STATUS_INVALID_STATE);
    CHECK(result.applied == 0U && CONTRACT_COUNT(registry) == 1U && CONTRACT_REVISION(registry) == current);
    CHECK(CONTRACT_ARCHIVE_RESTORE(registry, current, bytes, size, &result) == UMI_STATUS_OK);
    CHECK(result.applied == 2U && CONTRACT_COUNT(registry) == 2U && CONTRACT_REVISION(registry) == current + 1U);
    for (size_t index = 0U; index < 2U; ++index) {
        CHECK(CONTRACT_AT(registry, index, &output) == UMI_STATUS_OK);
        expected[index].revision = current + 1U;
        CHECK(ContractSnapshotEqual(&expected[index], &output));
    }
    /* Changed owners cannot export a mixed observation after measurement. */
    written = 77U;
    memcpy(sentinel, bytes, size);
    CHECK(CONTRACT_ARCHIVE_ENCODE(registry, observed, bytes, size, &written) == UMI_STATUS_INVALID_STATE);
    CHECK(written == 77U && memcmp(sentinel, bytes, size) == 0);
    free(sentinel); free(bytes);
    return 0;
}
static int ArchiveCollectionRefused(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT value = Record("retained"), expected;
    CHECK(CONTRACT_UPSERT(registry, &value) == UMI_STATUS_OK);
    CHECK(CONTRACT_AT(registry, 0U, &expected) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    size_t size = 0U;
    CHECK(CONTRACT_ARCHIVE_ENCODE(registry, revision, NULL, 0U, &size) == UMI_STATUS_OK);
    unsigned char *bytes = malloc(size + 1U);
    CHECK(bytes != NULL);
    CHECK(CONTRACT_ARCHIVE_ENCODE(registry, revision, bytes, size, &size) == UMI_STATUS_OK);
    UmiSnapshotBatchResult result;
    bytes[size] = 0U;
    CHECK(CONTRACT_ARCHIVE_RESTORE(registry, revision, bytes, size + 1U, &result) == UMI_STATUS_PARSE_ERROR);
    CHECK(result.applied == 0U && Unchanged(registry, 1U, revision, &expected) == 0);
    bytes[size - 1U] ^= 1U;
    CHECK(CONTRACT_ARCHIVE_RESTORE(registry, revision, bytes, size, &result) == UMI_STATUS_PARSE_ERROR);
    CHECK(result.applied == 0U && Unchanged(registry, 1U, revision, &expected) == 0);
    bytes[size - 1U] ^= 1U;
    CHECK(CONTRACT_ARCHIVE_RESTORE(registry, revision, NULL, size, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_ARCHIVE_RESTORE(NULL, revision, bytes, size, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(Unchanged(registry, 1U, revision, &expected) == 0);
    free(bytes);
    return 0;
}
static int ArchiveCollectionEmpty(CONTRACT_REGISTRY *registry)
{
    unsigned char bytes[UMI_VALUE_ARCHIVE_HEADER_SIZE + 8U];
    uint64_t revision = CONTRACT_REVISION(registry);
    size_t size = 0U;
    CHECK(CONTRACT_ARCHIVE_ENCODE(registry, revision, bytes, sizeof(bytes), &size) == UMI_STATUS_OK);
    CHECK(size == sizeof(bytes));
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_ARCHIVE_RESTORE(registry, revision, bytes, size, &result) == UMI_STATUS_OK);
    CHECK(CONTRACT_REVISION(registry) == revision && result.applied == 0U);
    CONTRACT_SNAPSHOT value = Record("to-clear");
    CHECK(CONTRACT_UPSERT(registry, &value) == UMI_STATUS_OK);
    revision = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_ARCHIVE_RESTORE(registry, revision, bytes, size, NULL) == UMI_STATUS_OK);
    CHECK(CONTRACT_COUNT(registry) == 0U && CONTRACT_REVISION(registry) == revision + 1U);
    return 0;
}
#endif
