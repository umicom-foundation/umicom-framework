/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/workspace_history.h
 *
 * PURPOSE:
 *   Define persistent workspace-history records for Studio and future Umicom products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_PLATFORM_WORKSPACE_HISTORY_H
#define UMICOM_PLATFORM_WORKSPACE_HISTORY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLATFORM_WORKSPACE_HISTORY_CAPACITY 512U

/**
 * Represent the workspace history snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkspaceHistorySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char root_uri[1024];
    char label[256];
    char profile[128];
    uint64_t last_opened;
    uint64_t duration_seconds;
    int trusted;
    int pinned;
    uint64_t revision;
} UmiWorkspaceHistorySnapshot;

/**
 * Represent the workspace history registry data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkspaceHistoryRegistry UmiWorkspaceHistoryRegistry;

/**
 * Initialise platform workspace history registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_platform_workspace_history_registry_create(UmiWorkspaceHistoryRegistry **out_registry);
/**
 * Release or reset state held by platform workspace history registry so the same storage
 * can be reused safely.
 */
void umi_platform_workspace_history_registry_destroy(UmiWorkspaceHistoryRegistry *registry);
/**
 * Provide the platform workspace history registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_platform_workspace_history_registry_upsert(UmiWorkspaceHistoryRegistry *registry, const UmiWorkspaceHistorySnapshot *item);
/**
 * Remove platform workspace history registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_platform_workspace_history_registry_remove(UmiWorkspaceHistoryRegistry *registry, const char *id);
/**
 * Find platform workspace history registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_workspace_history_registry_find(const UmiWorkspaceHistoryRegistry *registry, const char *id, UmiWorkspaceHistorySnapshot *out_item);
/**
 * Find platform workspace history registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_workspace_history_registry_at(const UmiWorkspaceHistoryRegistry *registry, size_t index, UmiWorkspaceHistorySnapshot *out_item);
/**
 * Provide the platform workspace history registry touch operation used by this module and
 * its client applications.
 */
UmiStatus umi_platform_workspace_history_registry_touch(UmiWorkspaceHistoryRegistry *registry,
                                                           const char *id,
                                                           uint64_t opened_at,
                                                           uint64_t additional_duration_seconds);
/**
 * Provide the platform workspace history registry set trusted operation used by this
 * module and its client applications.
 */
UmiStatus umi_platform_workspace_history_registry_set_trusted(UmiWorkspaceHistoryRegistry *registry,
                                                              const char *id,
                                                              int trusted);
/**
 * Return the number of records represented by platform workspace history registry without
 * changing their state.
 */
size_t umi_platform_workspace_history_registry_count(const UmiWorkspaceHistoryRegistry *registry);
/**
 * Provide the platform workspace history registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_platform_workspace_history_registry_revision(const UmiWorkspaceHistoryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
