/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/snapshot_contracts/safe_values.c
 * PURPOSE: Construct valid SDK records and read a consistent registry in small pages.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/bookmarks.h"
#include "umicom/sdk_runtime/component.h"
#include "umicom/test_runtime/artifact_locator.h"
#include <stdio.h>
#include <string.h>

/* Keep status handling visible in this lesson. A real UI can translate these
 * statuses into messages, but it must not publish a rejected partial value. */
static int ReadBookmarks(UmiBookmarkRegistry *registry)
{
    const uint64_t revision = umi_platform_bookmarks_registry_revision(registry);
    size_t offset = 0U;
    do {
        UmiBookmarkSnapshot items[2];
        UmiSnapshotPage page;
        UmiStatus status = umi_platform_bookmarks_registry_read_page(registry,
            revision, offset, items, 2U, &page);
        if (status != UMI_STATUS_OK) {
            /* INVALID_STATE requires a fresh observation, not continuation
             * with a newer token after already publishing earlier rows. */
            fprintf(stderr, "Bookmark observation refused: %d\n", (int)status);
            return 1;
        }
        for (size_t index = 0U; index < page.copied; ++index)
            printf("Bookmark: %s\n", items[index].id);
        if (!page.has_more) break;
        offset = page.next_offset;
    } while (1);
    return 0;
}

int main(void)
{
    UmiSdkRuntimeComponent component;
    UmiTestRuntimeArtifactLocator artifact;
    if (umi_sdk_runtime_component_init_checked(&component, "notes.application") != UMI_STATUS_OK ||
        umi_test_runtime_artifact_locator_init_checked(&artifact, "notes.test-report") != UMI_STATUS_OK)
        return 1;
    if (umi_sdk_runtime_component_set_detail(&component, "Installed Notes application") != UMI_STATUS_OK)
        return 1;
    if (umi_test_runtime_artifact_locator_set_name(&artifact, "test-report.txt") != UMI_STATUS_OK)
        return 1;
    unsigned char accepted[sizeof(component)];
    memcpy(accepted, &component, sizeof(component));
    if (umi_sdk_runtime_component_init_checked(&component, "") != UMI_STATUS_INVALID_ARGUMENT ||
        memcmp(accepted, &component, sizeof(component)) != 0) return 1;

    UmiBookmarkRegistry *registry = NULL;
    if (umi_platform_bookmarks_registry_create(&registry) != UMI_STATUS_OK) return 1;
    int result = 1;
    const char *names[] = {"notes.source", "notes.build", "notes.tests"};
    for (size_t index = 0U; index < 3U; ++index) {
        UmiBookmarkSnapshot item = {0};
        (void)snprintf(item.id, sizeof(item.id), "%s", names[index]);
        if (umi_platform_bookmarks_registry_upsert(registry, &item) != UMI_STATUS_OK) goto finish;
    }
    if (ReadBookmarks(registry) != 0) goto finish;
    /* A retained token cannot authorize reading a collection after mutation. */
    uint64_t observed = umi_platform_bookmarks_registry_revision(registry);
    if (umi_platform_bookmarks_registry_remove(registry, names[0]) != UMI_STATUS_OK) goto finish;
    UmiSnapshotPage page;
    if (umi_platform_bookmarks_registry_read_page(registry, observed, 0U, NULL, 0U, &page)
        != UMI_STATUS_INVALID_STATE) goto finish;
    puts("Checked construction and consistent page reads completed.");
    result = 0;
finish:
    umi_platform_bookmarks_registry_destroy(registry);
    return result;
}
