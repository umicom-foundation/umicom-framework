/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/snapshot_transfer_cases.h
 * PURPOSE: Verify complete captures, stale edit refusal and atomic collection replacement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_SNAPSHOT_TRANSFER_CASES_H
#define UMICOM_TEST_SNAPSHOT_TRANSFER_CASES_H

/* A capture must be useful after the source changes. Compare fields using the
 * domain's equality helper; C struct padding is not part of a record's value. */
static int TransferCapture(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT first = Record("alpha"), second = Record("beta");
    CONTRACT_SNAPSHOT copied[2] = {Record("sentinel-a"), Record("sentinel-b")};
    CONTRACT_SNAPSHOT sentinels[2] = {copied[0], copied[1]}, expected;
    UmiSnapshotCapture capture = {99U, 99U};
    CHECK(CONTRACT_CAPTURE(registry, NULL, 0U, &capture) == UMI_STATUS_OK);
    CHECK(capture.count == 0U && capture.revision == CONTRACT_REVISION(registry));
    CHECK(CONTRACT_UPSERT(registry, &first) == UMI_STATUS_OK);
    CHECK(CONTRACT_UPSERT(registry, &second) == UMI_STATUS_OK);
    CHECK(CONTRACT_CAPTURE(registry, copied, 1U, &capture) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(capture.count == 2U && capture.revision == CONTRACT_REVISION(registry));
    CHECK(ContractSnapshotEqual(&copied[0], &sentinels[0]));
    CHECK(ContractSnapshotEqual(&copied[1], &sentinels[1]));
    CHECK(CONTRACT_CAPTURE(registry, copied, 2U, &capture) == UMI_STATUS_OK);
    CHECK(CONTRACT_AT(registry, 0U, &expected) == UMI_STATUS_OK);
    CHECK(ContractSnapshotEqual(&copied[0], &expected));
    CHECK(CONTRACT_AT(registry, 1U, &expected) == UMI_STATUS_OK);
    CHECK(ContractSnapshotEqual(&copied[1], &expected));
    uint64_t observed = capture.revision;
    CHECK(CONTRACT_REMOVE(registry, "beta") == UMI_STATUS_OK);
    CHECK(capture.revision == observed && capture.count == 2U);
    CHECK(ContractSnapshotEqual(&copied[1], &expected));
    CHECK(CONTRACT_CAPTURE(NULL, copied, 2U, &capture) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(capture.count == 0U && capture.revision == 0U);
    CHECK(CONTRACT_CAPTURE(registry, NULL, 1U, &capture) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_CAPTURE(registry, copied, 2U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

/* Replacement is one publication, not a sequence that removes records before
 * learning that a later input is invalid. Its order follows the reviewed list. */
static int TransferReplace(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT old = Record("obsolete"), first = Record("alpha");
    CHECK(CONTRACT_UPSERT(registry, &old) == UMI_STATUS_OK);
    CHECK(CONTRACT_UPSERT(registry, &first) == UMI_STATUS_OK);
    UmiSnapshotCapture capture;
    CHECK(CONTRACT_CAPTURE(registry, NULL, 0U, &capture) == UMI_STATUS_OK);
    CONTRACT_SNAPSHOT proposed[2] = {Record("beta"), Record("alpha")};
    CONTRACT_SNAPSHOT retained[2] = {proposed[0], proposed[1]}, output;
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_REPLACE_CURRENT(registry, capture.revision, proposed, 2U, &result) == UMI_STATUS_OK);
    CHECK(result.applied == 2U && result.rejected_index == SIZE_MAX);
    CHECK(result.validation.issue == UMI_SNAPSHOT_VALID);
    CHECK(CONTRACT_COUNT(registry) == 2U && CONTRACT_REVISION(registry) == capture.revision + 1U);
    CHECK(CONTRACT_FIND(registry, "obsolete", &output) == UMI_STATUS_NOT_FOUND);
    for (size_t index = 0U; index < 2U; ++index) {
        CONTRACT_SNAPSHOT expected = proposed[index];
        expected.struct_size = (uint32_t)sizeof(expected);
        expected.api_version = CONTRACT_API_VERSION;
        CONTRACT_NORMALISE(&expected);
        expected.revision = capture.revision + 1U;
        CHECK(CONTRACT_AT(registry, index, &output) == UMI_STATUS_OK);
        CHECK(ContractSnapshotEqual(&expected, &output));
        CHECK(ContractSnapshotEqual(&proposed[index], &retained[index]));
    }
    CHECK(CONTRACT_REPLACE_CURRENT(registry, capture.revision, proposed, 2U, NULL) == UMI_STATUS_INVALID_STATE);
    CHECK(CONTRACT_REVISION(registry) == capture.revision + 1U);
    return 0;
}

/* Refusals must preserve data, ordering and the revision together. Malformed
 * strings, duplicate identities and oversized requests are separate failures. */
static int TransferReject(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT original = Record("alpha"), before;
    CHECK(CONTRACT_UPSERT(registry, &original) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "alpha", &before) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    CONTRACT_SNAPSHOT proposed[2] = {Record("beta"), Record("beta")};
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_REPLACE_CURRENT(registry, revision - 1U, proposed, 2U, &result) == UMI_STATUS_INVALID_STATE);
    CHECK(result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(Unchanged(registry, 1U, revision, &before) == 0);
    CHECK(CONTRACT_REPLACE_CURRENT(registry, revision, proposed, 2U, &result) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(result.applied == 0U && result.rejected_index == 1U);
    CHECK(result.validation.issue == UMI_SNAPSHOT_DUPLICATE_ID);
    CHECK(Unchanged(registry, 1U, revision, &before) == 0);
    proposed[1] = Record("gamma");
    memset(proposed[1].id, 'x', sizeof(proposed[1].id));
    CHECK(CONTRACT_REPLACE_CURRENT(registry, revision, proposed, 2U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result.applied == 0U && result.rejected_index == 1U);
    CHECK(result.validation.issue == UMI_SNAPSHOT_UNTERMINATED_TEXT);
    CHECK(Unchanged(registry, 1U, revision, &before) == 0);
    CHECK(CONTRACT_REPLACE_CURRENT(registry, revision, &original, SIZE_MAX, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result.rejected_index == SIZE_MAX && result.applied == 0U);
    CHECK(CONTRACT_REPLACE_CURRENT(registry, revision, NULL, 1U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_REPLACE_CURRENT(NULL, revision, &original, 1U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(Unchanged(registry, 1U, revision, &before) == 0);
    return 0;
}

/* Clearing a populated collection invalidates earlier observations. Repeating
 * a clear on an already empty collection does not consume another revision. */
static int TransferEmpty(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT first = Record("alpha");
    CHECK(CONTRACT_UPSERT(registry, &first) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_REPLACE_CURRENT(registry, revision, NULL, 0U, &result) == UMI_STATUS_OK);
    CHECK(result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(CONTRACT_COUNT(registry) == 0U && CONTRACT_REVISION(registry) == revision + 1U);
    CHECK(CONTRACT_REPLACE_CURRENT(registry, revision, NULL, 0U, &result) == UMI_STATUS_INVALID_STATE);
    CHECK(CONTRACT_REPLACE_CURRENT(registry, revision + 1U, NULL, 0U, NULL) == UMI_STATUS_OK);
    CHECK(CONTRACT_REVISION(registry) == revision + 1U);
    UmiSnapshotCapture capture;
    CHECK(CONTRACT_CAPTURE(registry, NULL, 0U, &capture) == UMI_STATUS_OK && capture.count == 0U);
    return 0;
}

#ifdef UMI_TEST_SNAPSHOT_ALLOCATION
/* Capture needs no staging allocation. A failed replacement allocation must
 * leave the old collection available for continued use and a deliberate retry. */
static int TransferAllocation(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT input = Record("alpha"), before;
    CHECK(CONTRACT_UPSERT(registry, &input) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "alpha", &before) == UMI_STATUS_OK);
    uint64_t revision = CONTRACT_REVISION(registry);
    UmiSnapshotCapture capture;
    fail_next_allocation = 1;
    CHECK(CONTRACT_CAPTURE(registry, NULL, 0U, &capture) == UMI_STATUS_OK);
    CHECK(fail_next_allocation == 1);
    UmiSnapshotBatchResult result;
    CHECK(CONTRACT_REPLACE_CURRENT(registry, revision, &input, 1U, &result) == UMI_STATUS_OUT_OF_MEMORY);
    CHECK(fail_next_allocation == 0 && result.applied == 0U && result.rejected_index == SIZE_MAX);
    CHECK(Unchanged(registry, 1U, revision, &before) == 0);
    fail_next_allocation = 1;
    CHECK(CONTRACT_REPLACE_CURRENT(registry, revision, NULL, 0U, &result) == UMI_STATUS_OUT_OF_MEMORY);
    CHECK(fail_next_allocation == 0);
    CHECK(Unchanged(registry, 1U, revision, &before) == 0);
    return 0;
}
#endif
#endif
