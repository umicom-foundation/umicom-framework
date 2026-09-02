/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/resource_centre.h
 *
 * PURPOSE:
 *   Define an owned resource-navigation centre combining recent items, bookmarks, locations, workspace history and queued file operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the resource centre data shared with callers of this public contract.
 */
typedef struct UmiResourceCentre UmiResourceCentre;

/**
 * Represent the resource centre snapshot data shared with callers of this public contract.
 */
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

/**
 * Initialise platform resource centre from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_platform_resource_centre_create(UmiResourceCentre **out_service);
/**
 * Release or reset state held by platform resource centre so the same storage can be
 * reused safely.
 */
void umi_platform_resource_centre_destroy(UmiResourceCentre *service);
/**
 * Provide the platform resource centre snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_platform_resource_centre_snapshot(const UmiResourceCentre *service, UmiResourceCentreSnapshot *out_snapshot);
/**
 * Provide the platform resource centre recent items operation used by this module and its
 * client applications.
 */
UmiRecentItemRegistry *umi_platform_resource_centre_recent_items(UmiResourceCentre *service);
/**
 * Provide the platform resource centre bookmarks operation used by this module and its
 * client applications.
 */
UmiBookmarkRegistry *umi_platform_resource_centre_bookmarks(UmiResourceCentre *service);
/**
 * Provide the platform resource centre locations operation used by this module and its
 * client applications.
 */
UmiResourceLocationRegistry *umi_platform_resource_centre_locations(UmiResourceCentre *service);
/**
 * Provide the platform resource centre workspaces operation used by this module and its
 * client applications.
 */
UmiWorkspaceHistoryRegistry *umi_platform_resource_centre_workspaces(UmiResourceCentre *service);
/**
 * Provide the platform resource centre operations operation used by this module and its
 * client applications.
 */
UmiFileOperationRegistry *umi_platform_resource_centre_operations(UmiResourceCentre *service);

#ifdef __cplusplus
}
#endif
#endif
