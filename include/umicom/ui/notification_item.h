/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/notification_item.h
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
#ifndef UMICOM_UI_NOTIFICATION_ITEM_H
#define UMICOM_UI_NOTIFICATION_ITEM_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_NOTIFICATION_ITEM_CAPACITY 4096U
#define UMI_UI_NOTIFICATION_ITEM_API_VERSION 1U

/**
 * Represent the ui notification item snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiNotificationItemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char title[256];
    char message[1024];
    char source[128];
    char action_id[128];
    uint64_t timestamp;
    int severity;
    int read;
    int sticky;
    uint64_t revision;
} UmiUiNotificationItemSnapshot;

/**
 * Represent the ui notification item registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiNotificationItemRegistry UmiUiNotificationItemRegistry;

/**
 * Initialise ui notification item registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_notification_item_registry_create(UmiUiNotificationItemRegistry **out_registry);
/**
 * Release or reset state held by ui notification item registry so the same storage can be
 * reused safely.
 */
void umi_ui_notification_item_registry_destroy(UmiUiNotificationItemRegistry *registry);
/**
 * Provide the ui notification item registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_notification_item_registry_upsert(UmiUiNotificationItemRegistry *registry, const UmiUiNotificationItemSnapshot *item);
/**
 * Remove ui notification item registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_notification_item_registry_remove(UmiUiNotificationItemRegistry *registry, const char *id);
/**
 * Find ui notification item registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_notification_item_registry_find(const UmiUiNotificationItemRegistry *registry, const char *id, UmiUiNotificationItemSnapshot *out_item);
/**
 * Find ui notification item registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_notification_item_registry_at(const UmiUiNotificationItemRegistry *registry, size_t index, UmiUiNotificationItemSnapshot *out_item);
/**
 * Return the number of records represented by ui notification item registry without
 * changing their state.
 */
size_t umi_ui_notification_item_registry_count(const UmiUiNotificationItemRegistry *registry);
/**
 * Provide the ui notification item registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_notification_item_registry_revision(const UmiUiNotificationItemRegistry *registry);
/**
 * Release or reset state held by ui notification item registry so the same storage can be
 * reused safely.
 */
void umi_ui_notification_item_registry_clear(UmiUiNotificationItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
