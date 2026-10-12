/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workspace_library/test_history_store.c
 * PURPOSE: Exercise native recent-item persistence without opening databases.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/recent_items.h"
#include "umicom/platform/local_replace.h"
#include <glib.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "Line %d: %s\n", __LINE__, #c); result = 1; goto cleanup; } } while (0)
int main(void)
{
    int result = 0, loaded = -1;
    UmiRecentItemRegistry *registry = NULL, *restored = NULL;
    char *directory = g_dir_make_tmp("umicom-recent-native-XXXXXX", NULL);
    char *file = directory ? g_build_filename(directory, "caf\xc3\xa9 history.txt", NULL) : NULL;
    char *parent = directory ? g_build_filename(directory, "not-created", NULL) : NULL;
    char *missingParent = parent ? g_build_filename(parent, "history.txt", NULL) : NULL;
    unsigned char *large = NULL;
    CHECK(directory && file && parent && missingParent);
    CHECK(UmiRecentItemsLoadLocal(file, &registry, &loaded) == UMI_STATUS_OK);
    CHECK(!loaded && registry && umi_platform_recent_items_registry_count(registry) == 0U);
    CHECK(!g_file_test(file, G_FILE_TEST_EXISTS));
    UmiRecentItemSnapshot item = {0}, output = {0};
    item.struct_size = (uint32_t)sizeof(item); item.api_version = 1U;
    memcpy(item.id, "a", sizeof("a")); memcpy(item.kind, "workspace-library", sizeof("workspace-library"));
    memcpy(item.label, "A | local library", sizeof("A | local library"));
    CHECK(strlen(file) < sizeof(item.uri)); memcpy(item.uri, file, strlen(file) + 1U);
    item.open_count = 4U; item.last_opened = 44U;
    CHECK(umi_platform_recent_items_registry_upsert(registry, &item) == UMI_STATUS_OK);
    CHECK(UmiRecentItemsSaveLocal(registry, file) == UMI_STATUS_OK);
    CHECK(UmiRecentItemsLoadLocal(file, &restored, &loaded) == UMI_STATUS_OK && loaded);
    CHECK(umi_platform_recent_items_registry_find(restored, "a", &output) == UMI_STATUS_OK);
    CHECK(strcmp(output.uri, file) == 0 && strcmp(output.label, item.label) == 0 && output.open_count == 4U);
    umi_platform_recent_items_registry_destroy(restored); restored = NULL;
    /* New native APIs refuse ambient relative paths and never create parents. */
    CHECK(UmiRecentItemsSaveLocal(registry, "relative.txt") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiRecentItemsLoadLocal("relative.txt", &restored, &loaded) == UMI_STATUS_INVALID_ARGUMENT && !restored);
    CHECK(UmiRecentItemsSaveLocal(registry, missingParent) != UMI_STATUS_OK);
    CHECK(!g_file_test(parent, G_FILE_TEST_EXISTS));
    CHECK(UmiRecentItemsLoadLocal(directory, &restored, &loaded) != UMI_STATUS_OK && !restored);
    /* A valid prefix followed by NUL must not hide damaged trailing bytes. */
    static const char concealed[] = "UMICOM_RECENT_ITEMS|1\n\0hidden record";
    CHECK(UmiLocalFileReplace(file, concealed, sizeof(concealed) - 1U) == UMI_STATUS_OK);
    CHECK(UmiRecentItemsLoadLocal(file, &restored, &loaded) == UMI_STATUS_PARSE_ERROR && !restored && !loaded);
    static const char duplicate[] = "UMICOM_RECENT_ITEMS|1\nR|61|62|63|64|1|1|0\nR|61|62|63|64|2|1|0\n";
    CHECK(UmiLocalFileReplace(file, duplicate, sizeof(duplicate) - 1U) == UMI_STATUS_OK);
    CHECK(UmiRecentItemsLoadLocal(file, &restored, &loaded) == UMI_STATUS_PARSE_ERROR && !restored);
    large = g_malloc0(4U * 1024U * 1024U + 1U);
    CHECK(UmiLocalFileReplace(file, large, 4U * 1024U * 1024U + 1U) == UMI_STATUS_OK);
    CHECK(UmiRecentItemsLoadLocal(file, &restored, &loaded) == UMI_STATUS_CAPACITY_EXCEEDED && !restored);
    /* Both APIs share a stable representation. Native saves stay readable by
     * an existing recent-item client, including the original counters. */
    CHECK(UmiRecentItemsSaveLocal(registry, file) == UMI_STATUS_OK);
    CHECK(umi_platform_recent_items_registry_load(file, &restored, &loaded) == UMI_STATUS_OK && loaded);
    CHECK(umi_platform_recent_items_registry_find(restored, "a", &output) == UMI_STATUS_OK && output.last_opened == 44U);
    /* A library surface must not evict pinned work or another feature's list. */
    memcpy(item.id, "b", sizeof("b")); item.pinned = 1;
    CHECK(umi_platform_recent_items_registry_upsert(registry, &item) == UMI_STATUS_OK);
    memcpy(item.id, "c", sizeof("c")); memcpy(item.kind, "project", sizeof("project")); item.pinned = 0;
    CHECK(umi_platform_recent_items_registry_upsert(registry, &item) == UMI_STATUS_OK);
    CHECK(UmiRecentItemsTrimKind(registry, "workspace-library", 0U) == UMI_STATUS_OK);
    CHECK(umi_platform_recent_items_registry_count(registry) == 2U);
    CHECK(umi_platform_recent_items_registry_find(registry, "a", &output) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_platform_recent_items_registry_find(registry, "b", &output) == UMI_STATUS_OK && output.pinned);
    CHECK(umi_platform_recent_items_registry_find(registry, "c", &output) == UMI_STATUS_OK && strcmp(output.kind, "project") == 0);
cleanup:
    umi_platform_recent_items_registry_destroy(restored); umi_platform_recent_items_registry_destroy(registry);
    g_free(large); if (file) (void)g_remove(file); if (directory) (void)g_rmdir(directory);
    g_free(file); g_free(parent); g_free(missingParent); g_free(directory);
    return result;
}
