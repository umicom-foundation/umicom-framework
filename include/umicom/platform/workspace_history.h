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

/* BEGINNER NOTE:
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

typedef struct UmiWorkspaceHistoryRegistry UmiWorkspaceHistoryRegistry;

UmiStatus umi_platform_workspace_history_registry_create(UmiWorkspaceHistoryRegistry **out_registry);
void umi_platform_workspace_history_registry_destroy(UmiWorkspaceHistoryRegistry *registry);
UmiStatus umi_platform_workspace_history_registry_upsert(UmiWorkspaceHistoryRegistry *registry, const UmiWorkspaceHistorySnapshot *item);
UmiStatus umi_platform_workspace_history_registry_remove(UmiWorkspaceHistoryRegistry *registry, const char *id);
UmiStatus umi_platform_workspace_history_registry_find(const UmiWorkspaceHistoryRegistry *registry, const char *id, UmiWorkspaceHistorySnapshot *out_item);
UmiStatus umi_platform_workspace_history_registry_at(const UmiWorkspaceHistoryRegistry *registry, size_t index, UmiWorkspaceHistorySnapshot *out_item);
UmiStatus umi_platform_workspace_history_registry_touch(UmiWorkspaceHistoryRegistry *registry,
                                                           const char *id,
                                                           uint64_t opened_at,
                                                           uint64_t additional_duration_seconds);
UmiStatus umi_platform_workspace_history_registry_set_trusted(UmiWorkspaceHistoryRegistry *registry,
                                                              const char *id,
                                                              int trusted);
size_t umi_platform_workspace_history_registry_count(const UmiWorkspaceHistoryRegistry *registry);
uint64_t umi_platform_workspace_history_registry_revision(const UmiWorkspaceHistoryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
