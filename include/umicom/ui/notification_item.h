/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/notification_item.h
 *
 * PURPOSE:
 *   Define an operational workbench service record for problems, output, progress, tasks, notifications, status and navigation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiUiNotificationItemRegistry UmiUiNotificationItemRegistry;

UmiStatus umi_ui_notification_item_registry_create(UmiUiNotificationItemRegistry **out_registry);
void umi_ui_notification_item_registry_destroy(UmiUiNotificationItemRegistry *registry);
UmiStatus umi_ui_notification_item_registry_upsert(UmiUiNotificationItemRegistry *registry, const UmiUiNotificationItemSnapshot *item);
UmiStatus umi_ui_notification_item_registry_remove(UmiUiNotificationItemRegistry *registry, const char *id);
UmiStatus umi_ui_notification_item_registry_find(const UmiUiNotificationItemRegistry *registry, const char *id, UmiUiNotificationItemSnapshot *out_item);
UmiStatus umi_ui_notification_item_registry_at(const UmiUiNotificationItemRegistry *registry, size_t index, UmiUiNotificationItemSnapshot *out_item);
size_t umi_ui_notification_item_registry_count(const UmiUiNotificationItemRegistry *registry);
uint64_t umi_ui_notification_item_registry_revision(const UmiUiNotificationItemRegistry *registry);
void umi_ui_notification_item_registry_clear(UmiUiNotificationItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
