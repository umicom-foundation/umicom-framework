/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/resource_centre.c
 *
 * PURPOSE:
 *   Implement an owned resource-navigation centre combining recent items, bookmarks, locations, workspace history and queued file operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation owns only its documented child models. Product-specific
 * business data and toolkit objects remain outside this reusable Framework layer.
 */
#include "umicom/platform/resource_centre.h"

#include <stdlib.h>
#include <string.h>

struct UmiResourceCentre {
    UmiRecentItemRegistry *recent;
    UmiBookmarkRegistry *bookmarks;
    UmiResourceLocationRegistry *locations;
    UmiWorkspaceHistoryRegistry *workspaces;
    UmiFileOperationRegistry *operations;
    uint64_t revision;
};

UmiStatus umi_platform_resource_centre_create(UmiResourceCentre **out_service)
{
    UmiResourceCentre *service;
    UmiStatus status = UMI_STATUS_OK;
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiResourceCentre *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->revision = 1U;
    if (status == UMI_STATUS_OK) status = umi_platform_recent_items_registry_create(&service->recent);
    if (status == UMI_STATUS_OK) status = umi_platform_bookmarks_registry_create(&service->bookmarks);
    if (status == UMI_STATUS_OK) status = umi_platform_resource_location_registry_create(&service->locations);
    if (status == UMI_STATUS_OK) status = umi_platform_workspace_history_registry_create(&service->workspaces);
    if (status == UMI_STATUS_OK) status = umi_platform_file_operation_queue_registry_create(&service->operations);
    if (status != UMI_STATUS_OK) {
        umi_platform_resource_centre_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_platform_resource_centre_destroy(UmiResourceCentre *service)
{
    if (service == NULL) return;
    umi_platform_file_operation_queue_registry_destroy(service->operations);
    umi_platform_workspace_history_registry_destroy(service->workspaces);
    umi_platform_resource_location_registry_destroy(service->locations);
    umi_platform_bookmarks_registry_destroy(service->bookmarks);
    umi_platform_recent_items_registry_destroy(service->recent);
    free(service);
}

UmiStatus umi_platform_resource_centre_snapshot(const UmiResourceCentre *service, UmiResourceCentreSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    out_snapshot->recent_count = umi_platform_recent_items_registry_count(service->recent);
    out_snapshot->bookmarks_count = umi_platform_bookmarks_registry_count(service->bookmarks);
    out_snapshot->locations_count = umi_platform_resource_location_registry_count(service->locations);
    out_snapshot->workspaces_count = umi_platform_workspace_history_registry_count(service->workspaces);
    out_snapshot->operations_count = umi_platform_file_operation_queue_registry_count(service->operations);
    out_snapshot->revision = service->revision;
    return UMI_STATUS_OK;
}

UmiRecentItemRegistry *umi_platform_resource_centre_recent_items(UmiResourceCentre *service)
{
    return service != NULL ? service->recent : NULL;
}

UmiBookmarkRegistry *umi_platform_resource_centre_bookmarks(UmiResourceCentre *service)
{
    return service != NULL ? service->bookmarks : NULL;
}

UmiResourceLocationRegistry *umi_platform_resource_centre_locations(UmiResourceCentre *service)
{
    return service != NULL ? service->locations : NULL;
}

UmiWorkspaceHistoryRegistry *umi_platform_resource_centre_workspaces(UmiResourceCentre *service)
{
    return service != NULL ? service->workspaces : NULL;
}

UmiFileOperationRegistry *umi_platform_resource_centre_operations(UmiResourceCentre *service)
{
    return service != NULL ? service->operations : NULL;
}
