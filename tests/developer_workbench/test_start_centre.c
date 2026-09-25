/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_start_centre.c
 *
 * PURPOSE:
 *   Verify Start Centre quick actions and recent projects.
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

#include "umicom/developer_workbench/recent_projects.h"
#include "umicom/developer_workbench/start_centre.h"
#include "umicom/platform/filesystem.h"

/* Copy a fixture path only after proving it fits the bounded recent-item URI.
 * The path-capacity mismatch is intentional: UMI_PATH_CAPACITY may exceed the
 * persisted recent-item URI capacity, so silent truncation is not acceptable. */
static void CopyRecentUri(char *destination, size_t capacity, const char *source)
{
    size_t length;

    assert(destination != NULL);
    assert(capacity > 0U);
    assert(source != NULL);
    length = strlen(source);
    assert(length < capacity);
    (void)memcpy(destination, source, length + 1U);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRecentItemRegistry *registry = NULL;
    UmiDeveloperWorkbenchStartCentreSnapshot snapshot;

    assert(umi_platform_recent_items_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_workbench_recent_projects_add(
        registry,
        "project.alpha",
        "C:/work/alpha",
        "Alpha",
        100U) == UMI_STATUS_OK);

    assert(umi_developer_workbench_start_centre_snapshot(
        registry, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.recent_count == 1U);
    assert(strcmp(snapshot.new_project_command, "project.new") == 0);
    assert(strcmp(snapshot.open_folder_command, "workspace.open-folder") == 0);

    umi_platform_recent_items_registry_destroy(registry);

    /*
     * Recent-work availability is tested separately so the established
     * project snapshot checks above remain unchanged.
     */
    {
        UmiRecentItemRegistry *workspaces = NULL;
        UmiDeveloperWorkbenchRecentWorkSnapshot recent_work;
        UmiRecentItemSnapshot item = {0};
        char temporary[UMI_PATH_CAPACITY];
        char available[UMI_PATH_CAPACITY];
        char missing[UMI_PATH_CAPACITY];

        assert(umi_fs_temp_directory(
            temporary, sizeof(temporary)) == UMI_STATUS_OK);
        assert(umi_fs_join(
            available,
            sizeof(available),
            temporary,
            "umicom-start-centre-available") == UMI_STATUS_OK);
        assert(umi_fs_join(
            missing,
            sizeof(missing),
            available,
            "missing") == UMI_STATUS_OK);

        (void)umi_fs_remove_tree(available);
        assert(umi_fs_make_directories(available) == UMI_STATUS_OK);
        assert(umi_platform_recent_items_registry_create(&workspaces) ==
               UMI_STATUS_OK);

        item.struct_size = (uint32_t)sizeof(item);
        item.api_version = 1U;
        (void)snprintf(item.id, sizeof(item.id), "%s", "workspace.available");
        /* The former snprintf accepted a 2048-byte path source for a 1024-byte
         * destination, so GCC correctly warned that the fixture URI could be
         * truncated. Retain it for review; the active copy rejects overflow. */
#if 0
        (void)snprintf(item.uri, sizeof(item.uri), "%s", available);
#endif
        CopyRecentUri(item.uri, sizeof(item.uri), available);
        (void)snprintf(item.label, sizeof(item.label), "%s", "Available");
        (void)snprintf(item.kind, sizeof(item.kind), "%s", "workspace");
        item.last_opened = 100U;
        item.open_count = 1U;
        assert(umi_platform_recent_items_registry_upsert(
            workspaces, &item) == UMI_STATUS_OK);

        item = (UmiRecentItemSnapshot){0};
        item.struct_size = (uint32_t)sizeof(item);
        item.api_version = 1U;
        (void)snprintf(item.id, sizeof(item.id), "%s", "workspace.missing");
        /* Preserve the previous bounded-format statement for review. The
         * checked byte copy below makes a too-long fixture path a test failure
         * instead of a warning-producing silent truncation. */
#if 0
        (void)snprintf(item.uri, sizeof(item.uri), "%s", missing);
#endif
        CopyRecentUri(item.uri, sizeof(item.uri), missing);
        (void)snprintf(item.label, sizeof(item.label), "%s", "Missing");
        (void)snprintf(item.kind, sizeof(item.kind), "%s", "workspace");
        item.last_opened = 200U;
        item.open_count = 2U;
        item.pinned = 1;
        assert(umi_platform_recent_items_registry_upsert(
            workspaces, &item) == UMI_STATUS_OK);

        assert(umi_developer_workbench_start_centre_recent_work_snapshot(
            workspaces, "workspace", 8U, &recent_work) == UMI_STATUS_OK);
        assert(recent_work.count == 2U);
        assert(recent_work.pinned_count == 1U);
        assert(recent_work.unavailable_count == 1U);
        assert(strcmp(
            recent_work.items[0].recent.id,
            "workspace.missing") == 0);
        assert(recent_work.items[0].available == 0);
        assert(strcmp(
            recent_work.items[1].recent.id,
            "workspace.available") == 0);
        assert(recent_work.items[1].available == 1);

        umi_platform_recent_items_registry_destroy(workspaces);
        assert(umi_fs_remove_tree(available) == UMI_STATUS_OK);
    }

    return 0;
}
