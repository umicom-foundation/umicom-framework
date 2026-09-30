/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/snapshot_scope_cases.h
 * PURPOSE: Exercise document replacement through each real public registry.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_SNAPSHOT_SCOPE_CASES_H
#define UMICOM_TEST_SNAPSHOT_SCOPE_CASES_H
static CONTRACT_SNAPSHOT ScopedRecord(const char *id, const char *document)
{
    CONTRACT_SNAPSHOT item = Record(id);
    (void)snprintf(item.document_id, sizeof(item.document_id), "%s", document);
    return item;
}

static int ScopeReplace(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT seed[3] = {ScopedRecord("old-a", "a"), ScopedRecord("foreign", "b"), ScopedRecord("old-c", "a")};
    CONTRACT_SNAPSHOT items[2] = {ScopedRecord("new-a", "a"), ScopedRecord("new-c", "a")}, foreign, output;
    CONTRACT_SNAPSHOT saved = items[0];
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_BATCH(registry, seed, 3U, NULL) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "foreign", &foreign) == UMI_STATUS_OK);
    const uint64_t revision = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, items, 2U, &result) == UMI_STATUS_OK);
    CHECK(result.applied == 2U && result.rejected_index == SIZE_MAX && result.validation.issue == UMI_SNAPSHOT_VALID);
    CHECK(CONTRACT_COUNT(registry) == 3U && CONTRACT_REVISION(registry) == revision + 4U);
    CHECK(CONTRACT_AT(registry, 0U, &output) == UMI_STATUS_OK && ContractSnapshotEqual(&output, &foreign));
    CHECK(CONTRACT_AT(registry, 1U, &output) == UMI_STATUS_OK && strcmp(output.id, "new-a") == 0);
    CHECK(CONTRACT_AT(registry, 2U, &output) == UMI_STATUS_OK && strcmp(output.id, "new-c") == 0);
    CHECK(CONTRACT_FIND(registry, "old-a", &output) == UMI_STATUS_NOT_FOUND);
    CHECK(CONTRACT_FIND(registry, "old-c", &output) == UMI_STATUS_NOT_FOUND);
    CHECK(ContractSnapshotEqual(&items[0], &saved));
    return 0;
}

static int ScopeEmpty(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT seed[2] = {ScopedRecord("old", "a"), ScopedRecord("foreign", "b")}, foreign;
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_BATCH(registry, seed, 2U, NULL) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "foreign", &foreign) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, NULL, 0U, &result) == UMI_STATUS_OK);
    CHECK(result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(Unchanged(registry, 1U, revision + 1U, &foreign) == 0);
    revision = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, NULL, 0U, NULL) == UMI_STATUS_OK);
    CHECK(Unchanged(registry, 1U, revision, &foreign) == 0);
    return 0;
}

static int ScopeReject(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT seed[2] = {ScopedRecord("old", "a"), ScopedRecord("foreign", "b")}, before, foreign;
    CONTRACT_SNAPSHOT items[2] = {ScopedRecord("one", "a"), ScopedRecord("two", "a")};
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_BATCH(registry, seed, 2U, NULL) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "old", &before) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "foreign", &foreign) == UMI_STATUS_OK);
    const uint64_t revision = CONTRACT_REVISION(registry);
    memset(items[1].document_id, 'x', sizeof(items[1].document_id));
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, items, 2U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result.applied == 0U && result.rejected_index == 1U && result.validation.issue == UMI_SNAPSHOT_UNTERMINATED_TEXT);
    CHECK(Unchanged(registry, 2U, revision, &before) == 0);
    items[1] = items[0];
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, items, 2U, &result) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(result.rejected_index == 1U && result.validation.issue == UMI_SNAPSHOT_DUPLICATE_ID);
    CHECK(Unchanged(registry, 2U, revision, &before) == 0);
    items[1] = ScopedRecord("two", "b");
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, items, 2U, &result) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(result.rejected_index == 1U && result.validation.issue == UMI_SNAPSHOT_SCOPE_MISMATCH);
    CHECK(strcmp(result.validation.field, "document_id") == 0);
    CHECK(Unchanged(registry, 2U, revision, &before) == 0);
    items[1] = ScopedRecord("foreign", "a");
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, items, 2U, &result) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(result.rejected_index == 1U && strcmp(result.validation.field, "id") == 0);
    CHECK(Unchanged(registry, 2U, revision, &before) == 0);
    CHECK(Unchanged(registry, 2U, revision, &foreign) == 0);
    return 0;
}

static int ScopeCapacity(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT item, before;
    char id[64];
    for (size_t i = 0U; i < (size_t)CONTRACT_CAPACITY; ++i) {
        (void)snprintf(id, sizeof(id), "seed-%zu", i);
        item = ScopedRecord(id, i == 0U ? "a" : "b");
        CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
    }
    CHECK(CONTRACT_FIND(registry, "seed-0", &before) == UMI_STATUS_OK);
    const uint64_t revision = CONTRACT_REVISION(registry);
    CONTRACT_SNAPSHOT items[2] = {ScopedRecord("replacement", "a"), ScopedRecord("overflow", "a")};
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, items, 2U, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(Unchanged(registry, CONTRACT_CAPACITY, revision, &before) == 0);
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, items, 1U, NULL) == UMI_STATUS_OK);
    CHECK(CONTRACT_COUNT(registry) == CONTRACT_CAPACITY && CONTRACT_REVISION(registry) == revision + 2U);
    CHECK(CONTRACT_FIND(registry, "replacement", &item) == UMI_STATUS_OK);
    return 0;
}

static int ScopeStale(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT item = ScopedRecord("old", "a"), before;
    UmiSnapshotBatchResult result;
    uint64_t stale = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "old", &before) == UMI_STATUS_OK);
    const uint64_t revision = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", stale, NULL, 0U, &result) == UMI_STATUS_INVALID_STATE);
    CHECK(result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(CONTRACT_REPLACE_DOCUMENT(NULL, "a", revision, NULL, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, NULL, revision, NULL, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "", revision, NULL, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, NULL, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, &item, SIZE_MAX, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
    char long_document[sizeof(item.document_id)];
    memset(long_document, 'x', sizeof(long_document));
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, long_document, revision, NULL, 0U, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(Unchanged(registry, 1U, revision, &before) == 0);
    return 0;
}

static int ScopeAlias(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT item = ScopedRecord("new", "a"), saved = item, output;
    const uint64_t revision = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, item.document_id, revision, &item, 1U, NULL) == UMI_STATUS_OK);
    CHECK(ContractSnapshotEqual(&item, &saved));
    CHECK(CONTRACT_FIND(registry, "new", &output) == UMI_STATUS_OK && strcmp(output.document_id, "a") == 0);
    CHECK(CONTRACT_REVISION(registry) == revision + 1U);
    return 0;
}
#ifdef UMI_TEST_SNAPSHOT_ALLOCATION
static int ScopeAllocation(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT item = ScopedRecord("old", "a"), before;
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "old", &before) == UMI_STATUS_OK);
    const uint64_t revision = CONTRACT_REVISION(registry);
    item = ScopedRecord("replacement", "a");
    fail_next_allocation = 1;
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, &item, 1U, &result) == UMI_STATUS_OUT_OF_MEMORY);
    CHECK(!fail_next_allocation && result.applied == 0U);
    CHECK(Unchanged(registry, 1U, revision, &before) == 0);
    fail_next_allocation = 1;
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, NULL, 0U, &result) == UMI_STATUS_OUT_OF_MEMORY);
    CHECK(!fail_next_allocation && result.applied == 0U);
    CHECK(Unchanged(registry, 1U, revision, &before) == 0);
    CHECK(CONTRACT_REPLACE_DOCUMENT(registry, "a", revision, &item, 1U, NULL) == UMI_STATUS_OK);
    return 0;
}
#endif
#endif
