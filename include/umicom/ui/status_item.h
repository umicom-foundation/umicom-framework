/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/status_item.h
 *
 * PURPOSE:
 *   Define an operational workbench service record for problems, output, progress, tasks, notifications, status and navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_UI_STATUS_ITEM_H
#define UMICOM_UI_STATUS_ITEM_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_STATUS_ITEM_CAPACITY 4096U
#define UMI_UI_STATUS_ITEM_API_VERSION 1U

/**
 * Represent the ui status item snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiStatusItemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char text[512];
    char tooltip[512];
    char command_id[128];
    char alignment[32];
    int visible;
    int32_t priority;
    uint64_t revision;
} UmiUiStatusItemSnapshot;

/**
 * Represent the ui status item registry data shared with callers of this public contract.
 */
typedef struct UmiUiStatusItemRegistry UmiUiStatusItemRegistry;

/**
 * Initialise ui status item registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_status_item_registry_create(UmiUiStatusItemRegistry **out_registry);
/**
 * Release or reset state held by ui status item registry so the same storage can be reused
 * safely.
 */
void umi_ui_status_item_registry_destroy(UmiUiStatusItemRegistry *registry);
/**
 * Provide the ui status item registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_status_item_registry_upsert(UmiUiStatusItemRegistry *registry, const UmiUiStatusItemSnapshot *item);
/**
 * Remove ui status item registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_status_item_registry_remove(UmiUiStatusItemRegistry *registry, const char *id);
/**
 * Find ui status item registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_status_item_registry_find(const UmiUiStatusItemRegistry *registry, const char *id, UmiUiStatusItemSnapshot *out_item);
/**
 * Find ui status item registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_status_item_registry_at(const UmiUiStatusItemRegistry *registry, size_t index, UmiUiStatusItemSnapshot *out_item);
/**
 * Return the number of records represented by ui status item registry without changing
 * their state.
 */
size_t umi_ui_status_item_registry_count(const UmiUiStatusItemRegistry *registry);
/**
 * Provide the ui status item registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_status_item_registry_revision(const UmiUiStatusItemRegistry *registry);
/**
 * Release or reset state held by ui status item registry so the same storage can be reused
 * safely.
 */
void umi_ui_status_item_registry_clear(UmiUiStatusItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
