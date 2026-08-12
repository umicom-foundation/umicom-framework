/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/view_state.h
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
#ifndef UMICOM_UI_VIEW_STATE_H
#define UMICOM_UI_VIEW_STATE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_VIEW_STATE_CAPACITY 4096U
#define UMI_UI_VIEW_STATE_API_VERSION 1U

typedef struct UmiUiViewStateSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char view_id[128];
    char workspace_id[128];
    char state_key[256];
    char state_value[2048];
    int persistent;
    uint64_t revision;
} UmiUiViewStateSnapshot;

typedef struct UmiUiViewStateRegistry UmiUiViewStateRegistry;

UmiStatus umi_ui_view_state_registry_create(UmiUiViewStateRegistry **out_registry);
void umi_ui_view_state_registry_destroy(UmiUiViewStateRegistry *registry);
UmiStatus umi_ui_view_state_registry_upsert(UmiUiViewStateRegistry *registry, const UmiUiViewStateSnapshot *item);
UmiStatus umi_ui_view_state_registry_remove(UmiUiViewStateRegistry *registry, const char *id);
UmiStatus umi_ui_view_state_registry_find(const UmiUiViewStateRegistry *registry, const char *id, UmiUiViewStateSnapshot *out_item);
UmiStatus umi_ui_view_state_registry_at(const UmiUiViewStateRegistry *registry, size_t index, UmiUiViewStateSnapshot *out_item);
size_t umi_ui_view_state_registry_count(const UmiUiViewStateRegistry *registry);
uint64_t umi_ui_view_state_registry_revision(const UmiUiViewStateRegistry *registry);
void umi_ui_view_state_registry_clear(UmiUiViewStateRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
