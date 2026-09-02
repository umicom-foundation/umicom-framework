/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_recent_items_persistence.c
 *
 * PURPOSE:
 *   Verify that recent work survives an atomic save and transactional load
 *   without losing paths, labels, counters or pinned state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/platform/atomic_file.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/recent_items.h"

int main(void)
{
    UmiRecentItemRegistry *source = NULL;
    UmiRecentItemRegistry *loaded = NULL;
    UmiRecentItemRegistry *rejected = NULL;
    UmiRecentItemSnapshot item = {0};
    UmiRecentItemSnapshot restored = {0};
    char temporary_directory[UMI_PATH_CAPACITY] = {0};
    char file_name[96] = {0};
    char path[UMI_PATH_CAPACITY] = {0};
    int was_loaded = 0;

    assert(umi_fs_temp_directory(
               temporary_directory, sizeof(temporary_directory)) ==
           UMI_STATUS_OK);
    (void)snprintf(file_name, sizeof(file_name),
                   "umicom-recent-items-%p.txt", (void *)&source);
    assert(umi_fs_join(path, sizeof(path), temporary_directory, file_name) ==
           UMI_STATUS_OK);

    assert(umi_platform_recent_items_registry_create(&source) == UMI_STATUS_OK);
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = 1U;
    (void)snprintf(item.id, sizeof(item.id), "%s", "studio-project");
    (void)snprintf(item.uri, sizeof(item.uri), "%s",
                   "file:///C:/work/Umicom Studio/sample project");
    (void)snprintf(item.label, sizeof(item.label), "%s",
                   "Sample | Studio Project");
    (void)snprintf(item.kind, sizeof(item.kind), "%s", "project");
    item.last_opened = 123456U;
    item.open_count = 7U;
    item.pinned = 1;
    assert(umi_platform_recent_items_registry_upsert(source, &item) ==
           UMI_STATUS_OK);

    assert(umi_platform_recent_items_registry_save(source, path) ==
           UMI_STATUS_OK);
    assert(umi_platform_recent_items_registry_load(
               path, &loaded, &was_loaded) == UMI_STATUS_OK);
    assert(was_loaded != 0);
    assert(umi_platform_recent_items_registry_find(
               loaded, "studio-project", &restored) == UMI_STATUS_OK);
    assert(strcmp(restored.uri, item.uri) == 0);
    assert(strcmp(restored.label, item.label) == 0);
    assert(restored.open_count == item.open_count);
    assert(restored.pinned != 0);

    /* A damaged file is rejected as one transaction; no partly populated
     * registry is returned to an application. */
    assert(umi_atomic_file_write_text(path, "not recent work\n") ==
           UMI_STATUS_OK);
    assert(umi_platform_recent_items_registry_load(
               path, &rejected, &was_loaded) == UMI_STATUS_PARSE_ERROR);
    assert(rejected == NULL);

    umi_platform_recent_items_registry_destroy(loaded);
    umi_platform_recent_items_registry_destroy(source);
    (void)remove(path);
    return 0;
}
