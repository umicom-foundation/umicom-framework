/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/view_state.h
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

/**
 * Represent the ui view state snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the ui view state registry data shared with callers of this public contract.
 */
typedef struct UmiUiViewStateRegistry UmiUiViewStateRegistry;

/**
 * Initialise ui view state registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_view_state_registry_create(UmiUiViewStateRegistry **out_registry);
/**
 * Release or reset state held by ui view state registry so the same storage can be reused
 * safely.
 */
void umi_ui_view_state_registry_destroy(UmiUiViewStateRegistry *registry);
/**
 * Provide the ui view state registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_state_registry_upsert(UmiUiViewStateRegistry *registry, const UmiUiViewStateSnapshot *item);
/**
 * Remove ui view state registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_view_state_registry_remove(UmiUiViewStateRegistry *registry, const char *id);
/**
 * Find ui view state registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_view_state_registry_find(const UmiUiViewStateRegistry *registry, const char *id, UmiUiViewStateSnapshot *out_item);
/**
 * Find ui view state registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_view_state_registry_at(const UmiUiViewStateRegistry *registry, size_t index, UmiUiViewStateSnapshot *out_item);
/**
 * Return the number of records represented by ui view state registry without changing
 * their state.
 */
size_t umi_ui_view_state_registry_count(const UmiUiViewStateRegistry *registry);
/**
 * Provide the ui view state registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_view_state_registry_revision(const UmiUiViewStateRegistry *registry);
/**
 * Release or reset state held by ui view state registry so the same storage can be reused
 * safely.
 */
void umi_ui_view_state_registry_clear(UmiUiViewStateRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
