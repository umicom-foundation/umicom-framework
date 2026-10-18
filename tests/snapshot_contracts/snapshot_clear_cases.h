/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * PURPOSE: Verify reviewed clearing through each real public value owner.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SNAPSHOT_CLEAR_CASES_H
#define UMICOM_SNAPSHOT_CLEAR_CASES_H

static int ClearCurrent(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT first = Record("alpha"), second = Record("beta"), retained;
    CHECK(CONTRACT_UPSERT(registry, &first) == UMI_STATUS_OK);
    CHECK(CONTRACT_UPSERT(registry, &second) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "alpha", &retained) == UMI_STATUS_OK);
    uint64_t reviewed = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_CLEAR_CURRENT(registry, reviewed) == UMI_STATUS_OK);
    CHECK(CONTRACT_COUNT(registry) == 0U && CONTRACT_REVISION(registry) == reviewed + 1U);
    /* A copied record remains valid after its original registry is cleared. */
    CONTRACT_SNAPSHOT output = retained;
    CHECK(CONTRACT_FIND(registry, "alpha", &output) == UMI_STATUS_NOT_FOUND);
    CHECK(ContractSnapshotEqual(&output, &retained));
    CHECK(CONTRACT_AT(registry, 0U, &output) == UMI_STATUS_NOT_FOUND);
    CHECK(ContractSnapshotEqual(&output, &retained));
    return 0;
}

static int ClearStale(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT first = Record("alpha"), second = Record("beta"), before, after;
    CHECK(CONTRACT_UPSERT(registry, &first) == UMI_STATUS_OK);
    uint64_t reviewed = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_UPSERT(registry, &second) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "beta", &before) == UMI_STATUS_OK);
    uint64_t current = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_CLEAR_CURRENT(registry, reviewed) == UMI_STATUS_INVALID_STATE);
    CHECK(CONTRACT_COUNT(registry) == 2U && CONTRACT_REVISION(registry) == current);
    CHECK(CONTRACT_FIND(registry, "beta", &after) == UMI_STATUS_OK);
    CHECK(ContractSnapshotEqual(&before, &after));
    CHECK(CONTRACT_FIND(registry, "alpha", &after) == UMI_STATUS_OK);
    return 0;
}

static int ClearEmpty(CONTRACT_REGISTRY *registry)
{
    uint64_t reviewed = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_CLEAR_CURRENT(registry, reviewed) == UMI_STATUS_OK);
    CHECK(CONTRACT_CLEAR_CURRENT(registry, reviewed) == UMI_STATUS_OK);
    CHECK(CONTRACT_COUNT(registry) == 0U && CONTRACT_REVISION(registry) == reviewed);
    /* Even an empty collection cannot accept an obsolete review. */
    CHECK(CONTRACT_CLEAR_CURRENT(registry, reviewed + 1U) == UMI_STATUS_INVALID_STATE);
    CHECK(CONTRACT_CLEAR_CURRENT(NULL, reviewed) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_COUNT(registry) == 0U && CONTRACT_REVISION(registry) == reviewed);
    return 0;
}

static int ClearRefill(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT item = Record("alpha"), before, after;
    CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "alpha", &before) == UMI_STATUS_OK);
    uint64_t reviewed = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_CLEAR_CURRENT(registry, reviewed) == UMI_STATUS_OK);
    CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "alpha", &after) == UMI_STATUS_OK);
    CHECK(after.revision > before.revision && CONTRACT_COUNT(registry) == 1U);
    CHECK(CONTRACT_CLEAR_CURRENT(registry, reviewed) == UMI_STATUS_INVALID_STATE);
    CHECK(CONTRACT_COUNT(registry) == 1U);
    /* Reusing an identifier does not restore the old review's authority. */
    CHECK(CONTRACT_CLEAR_CURRENT(registry, CONTRACT_REVISION(registry)) == UMI_STATUS_OK);
    return 0;
}

static int ClearPageInvalidation(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT item = Record("alpha"), before, output;
    CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
    CHECK(CONTRACT_FIND(registry, "alpha", &before) == UMI_STATUS_OK);
    uint64_t reviewed = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_CLEAR_CURRENT(registry, reviewed) == UMI_STATUS_OK);
    output = before;
    UmiSnapshotPage page;
    memset(&page, 0x5a, sizeof(page));
    UmiSnapshotPage prior;
    memcpy(&prior, &page, sizeof(prior));
    CHECK(CONTRACT_READ_PAGE(registry, reviewed, 0U, &output, 1U, &page) == UMI_STATUS_INVALID_STATE);
    CHECK(ContractSnapshotEqual(&output, &before) && memcmp(&page, &prior, sizeof(page)) == 0);
    CHECK(CONTRACT_READ_PAGE(registry, CONTRACT_REVISION(registry), 0U, NULL, 0U, &page) == UMI_STATUS_OK);
    return 0;
}
#endif
