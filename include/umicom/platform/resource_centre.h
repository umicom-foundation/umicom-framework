/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/resource_centre.h
 *
 * PURPOSE:
 *   Define an owned resource-navigation centre combining recent items, bookmarks, locations, workspace history and queued file operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract is toolkit-neutral and intended for reuse by Studio,
 * Trader, TMS and future Umicom applications without exposing GUI objects.
 */
#ifndef UMICOM_PLATFORM_RESOURCE_CENTRE_H
#define UMICOM_PLATFORM_RESOURCE_CENTRE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/recent_items.h"
#include "umicom/platform/bookmarks.h"
#include "umicom/platform/resource_location.h"
#include "umicom/platform/workspace_history.h"
#include "umicom/platform/file_operation_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiResourceCentre UmiResourceCentre;

typedef struct UmiResourceCentreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t recent_count;
    size_t bookmarks_count;
    size_t locations_count;
    size_t workspaces_count;
    size_t operations_count;
    uint64_t revision;
} UmiResourceCentreSnapshot;

UmiStatus umi_platform_resource_centre_create(UmiResourceCentre **out_service);
void umi_platform_resource_centre_destroy(UmiResourceCentre *service);
UmiStatus umi_platform_resource_centre_snapshot(const UmiResourceCentre *service, UmiResourceCentreSnapshot *out_snapshot);
UmiRecentItemRegistry *umi_platform_resource_centre_recent_items(UmiResourceCentre *service);
UmiBookmarkRegistry *umi_platform_resource_centre_bookmarks(UmiResourceCentre *service);
UmiResourceLocationRegistry *umi_platform_resource_centre_locations(UmiResourceCentre *service);
UmiWorkspaceHistoryRegistry *umi_platform_resource_centre_workspaces(UmiResourceCentre *service);
UmiFileOperationRegistry *umi_platform_resource_centre_operations(UmiResourceCentre *service);

#ifdef __cplusplus
}
#endif
#endif
