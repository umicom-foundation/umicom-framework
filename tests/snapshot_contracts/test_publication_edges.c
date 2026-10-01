/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_publication_edges.c
 * PURPOSE: Exercise rejected text edits and exhausted publication counters.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/text.h"
#include <stdio.h>
#include <string.h>

/* Compile one real value owner here so the fixture can place its private
 * counter at the boundary. Production headers gain no test-only mutation API,
 * and this target links the base library rather than a second bookmark owner. */
/* Only this translation unit substitutes allocation. Including stdlib first
 * keeps its declarations unchanged; the real owner still receives exactly the
 * same storage contract, with one deliberate refusal available to the test. */
#include <stdlib.h>
static int refuse_next_staging;
static void *PublicationAllocate(size_t size)
{
    if (refuse_next_staging) {
        refuse_next_staging = 0;
        return NULL;
    }
    return malloc(size);
}
#define malloc PublicationAllocate
#include "../../src/platform/bookmarks.c"
#undef malloc

#define REQUIRE(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; \
} } while (0)

static int TextPublication(void)
{
    char text[8] = "before";
    unsigned char before[sizeof(text)];
    uint64_t revision = 10U;
    memcpy(before, text, sizeof(text));
    REQUIRE(umi_text_update(text, sizeof(text), "oversized", &revision) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(revision == 10U && memcmp(before, text, sizeof(text)) == 0);
    REQUIRE(umi_text_update(text, sizeof(text), NULL, &revision) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_text_update(text, 0U, "ok", &revision) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_text_update(text, sizeof(text), "ok", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_text_update(NULL, sizeof(text), "ok", &revision) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(revision == 10U && memcmp(before, text, sizeof(text)) == 0);

    /* A bounded nonterminated input must fail without scanning beyond it. */
    char unterminated[sizeof(text)];
    memset(unterminated, 'x', sizeof(unterminated));
    REQUIRE(umi_text_update(text, sizeof(text), unterminated, &revision) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(revision == 10U && memcmp(before, text, sizeof(text)) == 0);
    REQUIRE(umi_text_update(text, sizeof(text), "1234567", &revision) == UMI_STATUS_OK);
    REQUIRE(strcmp(text, "1234567") == 0 && revision == 11U);
    REQUIRE(umi_text_update(text, sizeof(text), text + 2, &revision) == UMI_STATUS_OK);
    REQUIRE(strcmp(text, "34567") == 0 && revision == 12U);
    REQUIRE(umi_text_update(text, sizeof(text), text, &revision) == UMI_STATUS_OK);
    REQUIRE(strcmp(text, "34567") == 0 && revision == 13U);
    REQUIRE(umi_text_update(text, 1U, "", &revision) == UMI_STATUS_OK);
    REQUIRE(text[0] == '\0' && revision == 14U);

    revision = UINT64_MAX - 1U;
    REQUIRE(umi_text_update(text, sizeof(text), "last", &revision) == UMI_STATUS_OK);
    REQUIRE(revision == UINT64_MAX);
    memcpy(before, text, sizeof(text));
    REQUIRE(umi_text_update(text, sizeof(text), "next", &revision) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(revision == UINT64_MAX && memcmp(before, text, sizeof(text)) == 0);
    return 0;
}

static int RegistryExhaustion(void)
{
    UmiBookmarkRegistry *registry = NULL;
    REQUIRE(umi_platform_bookmarks_registry_create(&registry) == UMI_STATUS_OK);
    UmiBookmarkSnapshot input = {0}, output;
    (void)snprintf(input.id, sizeof(input.id), "%s", "notes");
    REQUIRE(umi_platform_bookmarks_registry_upsert(registry, &input) == UMI_STATUS_OK);
    registry->revision = UINT64_MAX - 1U;
    REQUIRE(umi_platform_bookmarks_registry_replace_if_current(
        registry, UINT64_MAX - 1U, &input, 1U, NULL) == UMI_STATUS_OK);
    REQUIRE(registry->revision == UINT64_MAX && registry->items[0].revision == UINT64_MAX);
    UmiSnapshotCapture capture;
    REQUIRE(umi_platform_bookmarks_registry_capture(registry, &output, 1U, &capture) == UMI_STATUS_OK);
    REQUIRE(capture.count == 1U && capture.revision == UINT64_MAX);
    REQUIRE(umi_platform_bookmarks_registry_upsert(registry, &input) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(umi_platform_bookmarks_registry_remove(registry, "notes") == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(umi_platform_bookmarks_registry_replace_if_current(
        registry, UINT64_MAX, &input, 1U, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(umi_platform_bookmarks_registry_replace_if_current(
        registry, UINT64_MAX, NULL, 0U, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(registry->revision == UINT64_MAX && registry->count == 1U);
    REQUIRE(strcmp(registry->items[0].id, "notes") == 0);

    /* The checked copy also refuses an inconsistent internal count before it
     * reads records; metadata is cleared and the caller's row stays intact. */
    registry->count = UMI_PLATFORM_BOOKMARKS_CAPACITY + 1U;
    REQUIRE(umi_platform_bookmarks_registry_capture(registry, &output, 1U, &capture) == UMI_STATUS_INVALID_STATE);
    REQUIRE(capture.count == 0U && capture.revision == 0U && strcmp(output.id, "notes") == 0);
    registry->count = 0U;
    REQUIRE(umi_platform_bookmarks_registry_replace_if_current(
        registry, UINT64_MAX, NULL, 0U, NULL) == UMI_STATUS_OK);
    REQUIRE(registry->revision == UINT64_MAX);
    umi_platform_bookmarks_registry_destroy(registry);
    return 0;
}


/* Several private mutations consume one public token, even at its boundary.
 * Never reset a live owner's counter to make a rejected operation succeed. */
static int RegistryEditBoundary(void)
{
    UmiBookmarkRegistry *registry = NULL;
    REQUIRE(umi_platform_bookmarks_registry_create(&registry) == UMI_STATUS_OK);
    UmiBookmarkSnapshot original = {0}, replacement = {0}, output;
    (void)snprintf(original.id, sizeof(original.id), "%s", "before");
    (void)snprintf(replacement.id, sizeof(replacement.id), "%s", "after");
    REQUIRE(umi_platform_bookmarks_registry_upsert(registry, &original) == UMI_STATUS_OK);
    registry->revision = UINT64_MAX - 1U;
    UmiBookmarkEdit edits[2] = {
        {UMI_SNAPSHOT_EDIT_REMOVE, original},
        {UMI_SNAPSHOT_EDIT_UPSERT, replacement}
    };
    UmiSnapshotBatchResult result;
    REQUIRE(umi_platform_bookmarks_registry_edit_if_current(
        registry, UINT64_MAX - 1U, edits, 2U, &result) == UMI_STATUS_OK);
    REQUIRE(result.applied == 2U && registry->revision == UINT64_MAX && registry->count == 1U);
    REQUIRE(umi_platform_bookmarks_registry_find(registry, "after", &output) == UMI_STATUS_OK);
    REQUIRE(output.revision == UINT64_MAX);
    REQUIRE(umi_platform_bookmarks_registry_edit_if_current(
        registry, UINT64_MAX, edits, 2U, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(result.applied == 0U && result.rejected_index == SIZE_MAX);
    REQUIRE(registry->revision == UINT64_MAX && registry->count == 1U);
    REQUIRE(strcmp(registry->items[0].id, "after") == 0);
    REQUIRE(umi_platform_bookmarks_registry_edit_if_current(
        registry, UINT64_MAX, NULL, 0U, NULL) == UMI_STATUS_OK);
    REQUIRE(umi_platform_bookmarks_registry_edit_if_current(
        registry, UINT64_MAX - 1U, NULL, 0U, NULL) == UMI_STATUS_INVALID_STATE);
    registry->count = UMI_PLATFORM_BOOKMARKS_CAPACITY + 1U;
    REQUIRE(umi_platform_bookmarks_registry_edit_if_current(
        registry, UINT64_MAX, NULL, 0U, NULL) == UMI_STATUS_INVALID_STATE);
    registry->count = 1U;
    umi_platform_bookmarks_registry_destroy(registry);
    return 0;
}

/* Fault injection is local to this fixture and works with the Windows build.
 * Production headers do not expose a special allocator or test-only setter. */
static int RegistryEditMemory(void)
{
    UmiBookmarkRegistry *registry = NULL;
    REQUIRE(umi_platform_bookmarks_registry_create(&registry) == UMI_STATUS_OK);
    UmiBookmarkSnapshot item = {0}, observed;
    (void)snprintf(item.id, sizeof(item.id), "%s", "notes");
    (void)snprintf(item.label, sizeof(item.label), "%s", "Original notes");
    REQUIRE(umi_platform_bookmarks_registry_upsert(registry, &item) == UMI_STATUS_OK);
    uint64_t revision = registry->revision;
    UmiBookmarkEdit edit = {UMI_SNAPSHOT_EDIT_REMOVE, item};
    UmiSnapshotBatchResult result;
    refuse_next_staging = 1;
    REQUIRE(umi_platform_bookmarks_registry_edit_if_current(
        registry, revision, NULL, 0U, NULL) == UMI_STATUS_OK);
    REQUIRE(refuse_next_staging == 1);
    REQUIRE(umi_platform_bookmarks_registry_edit_if_current(
        registry, revision, &edit, 1U, &result) == UMI_STATUS_OUT_OF_MEMORY);
    REQUIRE(refuse_next_staging == 0 && result.applied == 0U && result.rejected_index == SIZE_MAX);
    REQUIRE(registry->count == 1U && registry->revision == revision);
    REQUIRE(umi_platform_bookmarks_registry_find(registry, "notes", &observed) == UMI_STATUS_OK);
    REQUIRE(strcmp(observed.label, "Original notes") == 0 && observed.revision == revision);
    REQUIRE(umi_platform_bookmarks_registry_edit_if_current(
        registry, revision, &edit, 1U, NULL) == UMI_STATUS_OK);
    REQUIRE(registry->count == 0U && registry->revision == revision + 1U);
    umi_platform_bookmarks_registry_destroy(registry);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "edit-counter") == 0) return RegistryEditBoundary();
    if (strcmp(argv[1], "edit-memory") == 0) return RegistryEditMemory();
    if (strcmp(argv[1], "text") == 0) return TextPublication();
    if (strcmp(argv[1], "registry") == 0) return RegistryExhaustion();
    return 2;
}
