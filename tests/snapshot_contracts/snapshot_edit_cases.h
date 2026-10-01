/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/snapshot_edit_cases.h
 * PURPOSE: Check ordered mixed edits, rollback, capacity and stale reviews.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_SNAPSHOT_EDIT_CASES_H
#define UMICOM_TEST_SNAPSHOT_EDIT_CASES_H

/* Compare an independently captured observation by domain fields, never by
 * struct padding. A failed edit must preserve order as well as payload. */
static int EditUnchanged(CONTRACT_REGISTRY *registry, const CONTRACT_SNAPSHOT *before,
    size_t count, uint64_t revision)
{
    CHECK(CONTRACT_COUNT(registry) == count && CONTRACT_REVISION(registry) == revision);
    for (size_t index = 0U; index < count; ++index) {
        CONTRACT_SNAPSHOT after;
        CHECK(CONTRACT_AT(registry, index, &after) == UMI_STATUS_OK);
        CHECK(ContractSnapshotEqual(&before[index], &after));
    }
    return 0;
}

/* Existing single-row operations define domain normalisation. The transaction
 * must produce the same payload/order, with one shared publication revision. */
static int EditMixed(CONTRACT_REGISTRY *registry)
{
    CONTRACT_REGISTRY *reference = NULL;
    CHECK(CONTRACT_CREATE(&reference) == UMI_STATUS_OK);
    const char *ids[] = {"alpha", "beta", "gamma"};
    for (size_t index = 0U; index < 3U; ++index) {
        CONTRACT_SNAPSHOT item = Record(ids[index]);
        CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
        CHECK(CONTRACT_UPSERT(reference, &item) == UMI_STATUS_OK);
    }
    CONTRACT_EDIT edits[3] = {
        {UMI_SNAPSHOT_EDIT_REMOVE, Record("beta")},
        {UMI_SNAPSHOT_EDIT_UPSERT, Record("alpha")},
        {UMI_SNAPSHOT_EDIT_UPSERT, Record("delta")}
    };
    /* Some owners have no text payload beyond their identity. Use their
     * explicit scalar change so this still tests replacement of the same ID. */
#ifdef CONTRACT_CHANGE_PAYLOAD
    CONTRACT_CHANGE_PAYLOAD(&edits[1].item);
#else
    CHECK(FIELD_COUNT > 1U);
    ((char *)&edits[1].item)[contract_fields[1].offset] = 'e';
#endif
    unsigned char inputs_before[sizeof(edits)];
    memcpy(inputs_before, edits, sizeof(edits));
    CHECK(CONTRACT_REMOVE(reference, "beta") == UMI_STATUS_OK);
    CHECK(CONTRACT_UPSERT(reference, &edits[1].item) == UMI_STATUS_OK);
    CHECK(CONTRACT_UPSERT(reference, &edits[2].item) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_EDIT_CURRENT(registry, revision, edits, 3U, &result) == UMI_STATUS_OK);
    CHECK(result.applied == 3U && result.rejected_index == SIZE_MAX);
    CHECK(result.validation.issue == UMI_SNAPSHOT_VALID);
    CHECK(CONTRACT_REVISION(registry) == revision + 1U && CONTRACT_COUNT(registry) == 3U);
    CHECK(memcmp(inputs_before, edits, sizeof(edits)) == 0);
    for (size_t index = 0U; index < 3U; ++index) {
        CONTRACT_SNAPSHOT expected, actual;
        CHECK(CONTRACT_AT(reference, index, &expected) == UMI_STATUS_OK);
        CHECK(CONTRACT_AT(registry, index, &actual) == UMI_STATUS_OK);
        expected.revision = revision + 1U;
        CHECK(ContractSnapshotEqual(&expected, &actual));
    }
    CONTRACT_DESTROY(reference);
    return 0;
}

static int EditRollback(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT initial[2] = {Record("alpha"), Record("beta")}, before[2];
    CHECK(CONTRACT_BATCH(registry, initial, 2U, NULL) == UMI_STATUS_OK);
    UmiSnapshotCapture capture;
    CHECK(CONTRACT_CAPTURE(registry, before, 2U, &capture) == UMI_STATUS_OK);
    CONTRACT_EDIT edits[2] = {
        {UMI_SNAPSHOT_EDIT_UPSERT, Record("gamma")},
        {UMI_SNAPSHOT_EDIT_REMOVE, Record("missing")}
    };
    UmiSnapshotBatchResult result;
    /* The first insertion succeeds privately; failure of the later removal
     * must still retain every original row, its order and its revision. */
    CHECK(CONTRACT_EDIT_CURRENT(registry, capture.revision, edits, 2U, &result) == UMI_STATUS_NOT_FOUND);
    CHECK(result.applied == 0U && result.rejected_index == 1U);
    CHECK(EditUnchanged(registry, before, 2U, capture.revision) == 0);
    edits[1].kind = UMI_SNAPSHOT_EDIT_UPSERT;
    memset(edits[1].item.id, 'x', sizeof(edits[1].item.id));
    CHECK(CONTRACT_EDIT_CURRENT(registry, capture.revision, edits, 2U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result.applied == 0U && result.rejected_index == 1U);
    CHECK(result.validation.issue == UMI_SNAPSHOT_UNTERMINATED_TEXT);
    CHECK(EditUnchanged(registry, before, 2U, capture.revision) == 0);
    edits[1].item = Record("gamma");
    edits[1].kind = UMI_SNAPSHOT_EDIT_REMOVE;
    CHECK(CONTRACT_EDIT_CURRENT(registry, capture.revision, edits, 2U, &result) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(result.applied == 0U && result.rejected_index == 1U);
    CHECK(result.validation.issue == UMI_SNAPSHOT_DUPLICATE_ID);
    CHECK(EditUnchanged(registry, before, 2U, capture.revision) == 0);
    edits[1].item = Record("beta");
    edits[1].kind = (UmiSnapshotEditKind)99;
    CHECK(CONTRACT_EDIT_CURRENT(registry, capture.revision, edits, 2U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result.applied == 0U && result.rejected_index == 1U);
    CHECK(EditUnchanged(registry, before, 2U, capture.revision) == 0);
    return 0;
}

static int EditReview(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT initial = Record("alpha"), before;
    CHECK(CONTRACT_UPSERT(registry, &initial) == UMI_STATUS_OK);
    CHECK(CONTRACT_AT(registry, 0U, &before) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    CONTRACT_EDIT edit = {UMI_SNAPSHOT_EDIT_REMOVE, Record("alpha")};
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_EDIT_CURRENT(registry, revision - 1U, &edit, 1U, &result) == UMI_STATUS_INVALID_STATE);
    CHECK(result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(EditUnchanged(registry, &before, 1U, revision) == 0);
    CHECK(CONTRACT_EDIT_CURRENT(registry, revision - 1U, NULL, 0U, NULL) == UMI_STATUS_INVALID_STATE);
    CHECK(CONTRACT_EDIT_CURRENT(registry, revision, NULL, 0U, &result) == UMI_STATUS_OK);
    CHECK(result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(CONTRACT_EDIT_CURRENT(registry, revision, NULL, 1U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_EDIT_CURRENT(NULL, revision, &edit, 1U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_EDIT_CURRENT(registry, revision, &edit, SIZE_MAX, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(EditUnchanged(registry, &before, 1U, revision) == 0);
    /* Removal needs only a bounded identity; unrelated, even malformed text
     * is deliberately ignored instead of forcing clients to rebuild a row. */
    memset(&edit.item, 0xff, sizeof(edit.item));
    (void)snprintf(edit.item.id, sizeof(edit.item.id), "%s", "alpha");
    CHECK(CONTRACT_EDIT_CURRENT(registry, revision, &edit, 1U, NULL) == UMI_STATUS_OK);
    CHECK(CONTRACT_COUNT(registry) == 0U && CONTRACT_REVISION(registry) == revision + 1U);
    return 0;
}

static int EditCapacity(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT *before = calloc(CONTRACT_CAPACITY, sizeof(*before));
    CONTRACT_EDIT *edits = calloc((size_t)CONTRACT_CAPACITY * 2U, sizeof(*edits));
    CHECK(before != NULL && edits != NULL);
    for (size_t index = 0U; index < CONTRACT_CAPACITY; ++index) {
        char id[32];
        (void)snprintf(id, sizeof(id), "old-%zu", index);
        CONTRACT_SNAPSHOT item = Record(id);
        CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
        edits[index].kind = UMI_SNAPSHOT_EDIT_REMOVE;
        edits[index].item = item;
        (void)snprintf(id, sizeof(id), "new-%zu", index);
        edits[CONTRACT_CAPACITY + index].kind = UMI_SNAPSHOT_EDIT_UPSERT;
        edits[CONTRACT_CAPACITY + index].item = Record(id);
    }
    UmiSnapshotCapture capture;
    CHECK(CONTRACT_CAPTURE(registry, before, CONTRACT_CAPACITY, &capture) == UMI_STATUS_OK);
    CONTRACT_EDIT full[2] = {
        {UMI_SNAPSHOT_EDIT_UPSERT, Record("extra")},
        {UMI_SNAPSHOT_EDIT_REMOVE, Record("old-0")}
    };
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_EDIT_CURRENT(registry, capture.revision, full, 2U, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result.applied == 0U && result.rejected_index == 0U);
    CHECK(EditUnchanged(registry, before, CONTRACT_CAPACITY, capture.revision) == 0);
    /* Removing first makes room. Exercise the complete documented edit bound
     * rather than assuming a transaction may hold only one stored capacity. */
    CHECK(CONTRACT_EDIT_CURRENT(registry, capture.revision, edits,
        (size_t)CONTRACT_CAPACITY * 2U, &result) == UMI_STATUS_OK);
    CHECK(result.applied == (size_t)CONTRACT_CAPACITY * 2U && result.rejected_index == SIZE_MAX);
    CHECK(CONTRACT_COUNT(registry) == CONTRACT_CAPACITY && CONTRACT_REVISION(registry) == capture.revision + 1U);
    for (size_t index = 0U; index < CONTRACT_CAPACITY; ++index) {
        CONTRACT_SNAPSHOT actual, expected = edits[CONTRACT_CAPACITY + index].item;
        CHECK(CONTRACT_FIND(registry, edits[index].item.id, &actual) == UMI_STATUS_NOT_FOUND);
        CHECK(CONTRACT_AT(registry, index, &actual) == UMI_STATUS_OK);
        expected.struct_size = (uint32_t)sizeof(expected);
        expected.api_version = CONTRACT_API_VERSION;
        CONTRACT_NORMALISE(&expected);
        expected.revision = capture.revision + 1U;
        CHECK(ContractSnapshotEqual(&expected, &actual));
    }
    free(edits);
    free(before);
    return 0;
}

#ifdef UMI_TEST_SNAPSHOT_ALLOCATION
static int EditAllocation(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT input = Record("alpha"), before;
    CHECK(CONTRACT_UPSERT(registry, &input) == UMI_STATUS_OK);
    CHECK(CONTRACT_AT(registry, 0U, &before) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    CONTRACT_EDIT edits[2] = {
        {UMI_SNAPSHOT_EDIT_REMOVE, Record("alpha")},
        {UMI_SNAPSHOT_EDIT_UPSERT, Record("beta")}
    };
    UmiSnapshotBatchResult result;
    fail_next_allocation = 1;
    CHECK(CONTRACT_EDIT_CURRENT(registry, revision, NULL, 0U, NULL) == UMI_STATUS_OK);
    CHECK(fail_next_allocation == 1);
    CHECK(CONTRACT_EDIT_CURRENT(registry, revision, edits, 2U, &result) == UMI_STATUS_OUT_OF_MEMORY);
    CHECK(fail_next_allocation == 0 && result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(EditUnchanged(registry, &before, 1U, revision) == 0);
    CHECK(CONTRACT_EDIT_CURRENT(registry, revision, edits, 2U, NULL) == UMI_STATUS_OK);
    return 0;
}
#endif
#endif
