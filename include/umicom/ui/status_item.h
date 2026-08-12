/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/status_item.h
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

typedef struct UmiUiStatusItemRegistry UmiUiStatusItemRegistry;

UmiStatus umi_ui_status_item_registry_create(UmiUiStatusItemRegistry **out_registry);
void umi_ui_status_item_registry_destroy(UmiUiStatusItemRegistry *registry);
UmiStatus umi_ui_status_item_registry_upsert(UmiUiStatusItemRegistry *registry, const UmiUiStatusItemSnapshot *item);
UmiStatus umi_ui_status_item_registry_remove(UmiUiStatusItemRegistry *registry, const char *id);
UmiStatus umi_ui_status_item_registry_find(const UmiUiStatusItemRegistry *registry, const char *id, UmiUiStatusItemSnapshot *out_item);
UmiStatus umi_ui_status_item_registry_at(const UmiUiStatusItemRegistry *registry, size_t index, UmiUiStatusItemSnapshot *out_item);
size_t umi_ui_status_item_registry_count(const UmiUiStatusItemRegistry *registry);
uint64_t umi_ui_status_item_registry_revision(const UmiUiStatusItemRegistry *registry);
void umi_ui_status_item_registry_clear(UmiUiStatusItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
