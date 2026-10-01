/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/snapshot_page_cases.h
 * PURPOSE: Exercise stable paged reads through every typed registry owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_SNAPSHOT_PAGE_CASES_H
#define UMICOM_TEST_SNAPSHOT_PAGE_CASES_H
/* Use accepted rows from the public at() API for expected payloads. Domains
 * can normalize fields during insertion; a page must preserve those values. */
static int PageRead(CONTRACT_REGISTRY *registry)
{
    const char *ids[] = {"page-first", "page-second", "page-third"};
    for (size_t index = 0U; index < 3U; ++index) {
        CONTRACT_SNAPSHOT item = Record(ids[index]);
        CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
    }
    CONTRACT_SNAPSHOT items[2], expected;
    UmiSnapshotPage page;
    const uint64_t revision = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_READ_PAGE(registry, revision, 0U, items, 2U, &page) == UMI_STATUS_OK);
    CHECK(page.revision == revision && page.total_count == 3U && page.offset == 0U &&
        page.copied == 2U && page.next_offset == 2U && page.has_more);
    for (size_t index = 0U; index < 2U; ++index) {
        CHECK(CONTRACT_AT(registry, index, &expected) == UMI_STATUS_OK);
        CHECK(ContractSnapshotEqual(&items[index], &expected));
    }
    unsigned char untouched[sizeof(items[1])];
    memcpy(untouched, &items[1], sizeof(items[1]));
    CHECK(CONTRACT_READ_PAGE(registry, revision, page.next_offset, items, 2U, &page) == UMI_STATUS_OK);
    CHECK(page.revision == revision && page.total_count == 3U && page.offset == 2U &&
        page.copied == 1U && page.next_offset == 3U && !page.has_more);
    CHECK(CONTRACT_AT(registry, 2U, &expected) == UMI_STATUS_OK);
    CHECK(ContractSnapshotEqual(&items[0], &expected));
    CHECK(memcmp(untouched, &items[1], sizeof(items[1])) == 0);
    CHECK(CONTRACT_READ_PAGE(registry, revision, 3U, NULL, 0U, &page) == UMI_STATUS_OK);
    CHECK(page.copied == 0U && page.offset == 3U && page.next_offset == 3U && !page.has_more);
    CHECK(CONTRACT_REVISION(registry) == revision && CONTRACT_COUNT(registry) == 3U);
    return 0;
}

static int PageRefused(CONTRACT_REGISTRY *registry)
{
    CONTRACT_SNAPSHOT item = Record("page-existing"), output;
    CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
    const uint64_t revision = CONTRACT_REVISION(registry);
    UmiSnapshotPage page;
    CHECK(CONTRACT_READ_PAGE(registry, revision, 0U, &output, 1U, &page) == UMI_STATUS_OK);
    unsigned char before[sizeof(output)], before_page[sizeof(page)];
    memcpy(before, &output, sizeof(output));
    memcpy(before_page, &page, sizeof(page));
    CHECK(CONTRACT_READ_PAGE(NULL, revision, 0U, &output, 1U, &page) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_READ_PAGE(registry, revision, 0U, NULL, 1U, &page) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_READ_PAGE(registry, revision, 0U, &output, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_READ_PAGE(registry, revision, 2U, &output, 1U, &page) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(CONTRACT_READ_PAGE(registry, revision, SIZE_MAX, &output, 1U, &page) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before, &output, sizeof(output)) == 0 && memcmp(before_page, &page, sizeof(page)) == 0);
    /* A deletion or another write invalidates even an empty last-page read. */
    CHECK(CONTRACT_REMOVE(registry, item.id) == UMI_STATUS_OK);
    CHECK(CONTRACT_READ_PAGE(registry, revision, 0U, &output, 1U, &page) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(before, &output, sizeof(output)) == 0 && memcmp(before_page, &page, sizeof(page)) == 0);
    CHECK(CONTRACT_COUNT(registry) == 0U && CONTRACT_REVISION(registry) == revision + 1U);
    return 0;
}

static int PageMetadata(CONTRACT_REGISTRY *registry)
{
    UmiSnapshotPage page;
    uint64_t revision = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_READ_PAGE(registry, revision, 0U, NULL, 0U, &page) == UMI_STATUS_OK);
    CHECK(page.revision == revision && page.total_count == 0U && page.copied == 0U &&
        page.offset == 0U && page.next_offset == 0U && !page.has_more);
    CONTRACT_SNAPSHOT item = Record("page-metadata");
    CHECK(CONTRACT_UPSERT(registry, &item) == UMI_STATUS_OK);
    revision = CONTRACT_REVISION(registry);
    CHECK(CONTRACT_READ_PAGE(registry, revision, 0U, NULL, 0U, &page) == UMI_STATUS_OK);
    CHECK(page.revision == revision && page.total_count == 1U && page.offset == 0U &&
        page.copied == 0U && page.next_offset == 0U && page.has_more);
    /* The size query never consumes a row or advances the owner's token. */
    CHECK(CONTRACT_REVISION(registry) == revision && CONTRACT_COUNT(registry) == 1U);
    CHECK(CONTRACT_READ_PAGE(registry, revision, 0U, &item, 1U, &page) == UMI_STATUS_OK);
    CHECK(page.copied == 1U && page.next_offset == 1U && !page.has_more);
    return 0;
}
#endif
