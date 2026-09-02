/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/context_menu.h
 *
 * PURPOSE:
 *   Define context-menu contributions with commands, grouping and context expressions.
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
#ifndef UMICOM_UI_CONTEXT_MENU_H
#define UMICOM_UI_CONTEXT_MENU_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_CONTEXT_MENU_CAPACITY 1024U

/**
 * Represent the ui context menu item snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiContextMenuItemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char menu_id[128];
    char command_id[128];
    char label[256];
    char when_expression[256];
    char group[128];
    int visible;
    int enabled;
    int separator_before;
    int32_t order;
    uint64_t revision;
} UmiUiContextMenuItemSnapshot;

/**
 * Represent the ui context menu item registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiContextMenuItemRegistry UmiUiContextMenuItemRegistry;

/**
 * Initialise ui context menu registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_context_menu_registry_create(UmiUiContextMenuItemRegistry **out_registry);
/**
 * Release or reset state held by ui context menu registry so the same storage can be
 * reused safely.
 */
void umi_ui_context_menu_registry_destroy(UmiUiContextMenuItemRegistry *registry);
/**
 * Provide the ui context menu registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_context_menu_registry_upsert(UmiUiContextMenuItemRegistry *registry, const UmiUiContextMenuItemSnapshot *item);
/**
 * Remove ui context menu registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_context_menu_registry_remove(UmiUiContextMenuItemRegistry *registry, const char *id);
/**
 * Find ui context menu registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_context_menu_registry_find(const UmiUiContextMenuItemRegistry *registry, const char *id, UmiUiContextMenuItemSnapshot *out_item);
/**
 * Find ui context menu registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_context_menu_registry_at(const UmiUiContextMenuItemRegistry *registry, size_t index, UmiUiContextMenuItemSnapshot *out_item);
/**
 * Return the number of records represented by ui context menu registry without changing
 * their state.
 */
size_t umi_ui_context_menu_registry_count(const UmiUiContextMenuItemRegistry *registry);
/**
 * Provide the ui context menu registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_context_menu_registry_revision(const UmiUiContextMenuItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
