/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_resource_navigation.c
 *
 * PURPOSE:
 *   Exercise recent resources, bookmarks, workspace history and queued file
 *   operations used by IDE and file-manager style products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <string.h>

#include "umicom/platform/bookmarks.h"
#include "umicom/platform/file_operation_queue.h"
#include "umicom/platform/recent_items.h"
#include "umicom/platform/workspace_history.h"

int main(void)
{
    UmiRecentItemRegistry *recent = NULL;
    UmiRecentItemSnapshot recent_item = {0};
    UmiBookmarkRegistry *bookmarks = NULL;
    UmiBookmarkSnapshot bookmark = {0};
    UmiWorkspaceHistoryRegistry *history = NULL;
    UmiWorkspaceHistorySnapshot workspace = {0};
    UmiFileOperationRegistry *operations = NULL;
    UmiFileOperationSnapshot operation = {0};

    if (umi_platform_recent_items_registry_create(&recent) != UMI_STATUS_OK) return 1;
    (void)strcpy(recent_item.id, "workspace");
    (void)strcpy(recent_item.uri, "file:///workspace");
    if (umi_platform_recent_items_registry_upsert(recent, &recent_item) != UMI_STATUS_OK) return 2;
    if (umi_platform_recent_items_registry_touch(recent, "workspace", 123U) != UMI_STATUS_OK) return 3;
    if (umi_platform_recent_items_registry_set_pinned(recent, "workspace", 1) != UMI_STATUS_OK) return 4;
    if (umi_platform_recent_items_registry_find(recent, "workspace", &recent_item) != UMI_STATUS_OK ||
        recent_item.open_count != 1U || recent_item.last_opened != 123U || !recent_item.pinned) return 5;

    if (umi_platform_bookmarks_registry_create(&bookmarks) != UMI_STATUS_OK) return 6;
    (void)strcpy(bookmark.id, "home");
    (void)strcpy(bookmark.uri, "file:///home");
    if (umi_platform_bookmarks_registry_upsert(bookmarks, &bookmark) != UMI_STATUS_OK) return 7;

    if (umi_platform_workspace_history_registry_create(&history) != UMI_STATUS_OK) return 8;
    (void)strcpy(workspace.id, "studio");
    (void)strcpy(workspace.root_uri, "file:///workspace");
    if (umi_platform_workspace_history_registry_upsert(history, &workspace) != UMI_STATUS_OK) return 9;
    if (umi_platform_workspace_history_registry_touch(history, "studio", 200U, 45U) != UMI_STATUS_OK) return 10;
    if (umi_platform_workspace_history_registry_set_trusted(history, "studio", 1) != UMI_STATUS_OK) return 11;
    if (umi_platform_workspace_history_registry_find(history, "studio", &workspace) != UMI_STATUS_OK ||
        workspace.duration_seconds != 45U || !workspace.trusted) return 12;

    if (umi_platform_file_operation_queue_registry_create(&operations) != UMI_STATUS_OK) return 13;
    (void)strcpy(operation.id, "copy-1");
    (void)strcpy(operation.operation, "copy");
    operation.bytes_total = 100U;
    if (umi_platform_file_operation_queue_registry_upsert(operations, &operation) != UMI_STATUS_OK) return 14;
    if (umi_platform_file_operation_queue_registry_update_progress(
            operations, "copy-1", 150U, 2, "") != UMI_STATUS_OK) return 15;
    if (umi_platform_file_operation_queue_registry_find(operations, "copy-1", &operation) != UMI_STATUS_OK ||
        operation.bytes_done != 100U || operation.state != 2) return 16;

    umi_platform_file_operation_queue_registry_destroy(operations);
    umi_platform_workspace_history_registry_destroy(history);
    umi_platform_bookmarks_registry_destroy(bookmarks);
    umi_platform_recent_items_registry_destroy(recent);
    return 0;
}
