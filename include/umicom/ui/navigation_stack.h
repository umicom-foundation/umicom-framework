/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/navigation_stack.h
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
#ifndef UMICOM_UI_NAVIGATION_STACK_H
#define UMICOM_UI_NAVIGATION_STACK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_NAVIGATION_STACK_CAPACITY 4096U
#define UMI_UI_NAVIGATION_STACK_API_VERSION 1U

/**
 * Represent the ui navigation entry snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiNavigationEntrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char uri[1024];
    char label[256];
    uint32_t line;
    uint32_t column;
    uint64_t visited_at;
    int current;
    uint64_t revision;
} UmiUiNavigationEntrySnapshot;

/**
 * Represent the ui navigation entry registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiNavigationEntryRegistry UmiUiNavigationEntryRegistry;

/**
 * Initialise ui navigation stack registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_navigation_stack_registry_create(UmiUiNavigationEntryRegistry **out_registry);
/**
 * Release or reset state held by ui navigation stack registry so the same storage can be
 * reused safely.
 */
void umi_ui_navigation_stack_registry_destroy(UmiUiNavigationEntryRegistry *registry);
/**
 * Provide the ui navigation stack registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_navigation_stack_registry_upsert(UmiUiNavigationEntryRegistry *registry, const UmiUiNavigationEntrySnapshot *item);
/**
 * Remove ui navigation stack registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_navigation_stack_registry_remove(UmiUiNavigationEntryRegistry *registry, const char *id);
/**
 * Find ui navigation stack registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_navigation_stack_registry_find(const UmiUiNavigationEntryRegistry *registry, const char *id, UmiUiNavigationEntrySnapshot *out_item);
/**
 * Find ui navigation stack registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_navigation_stack_registry_at(const UmiUiNavigationEntryRegistry *registry, size_t index, UmiUiNavigationEntrySnapshot *out_item);
/**
 * Return the number of records represented by ui navigation stack registry without
 * changing their state.
 */
size_t umi_ui_navigation_stack_registry_count(const UmiUiNavigationEntryRegistry *registry);
/**
 * Provide the ui navigation stack registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_navigation_stack_registry_revision(const UmiUiNavigationEntryRegistry *registry);
/**
 * Release or reset state held by ui navigation stack registry so the same storage can be
 * reused safely.
 */
void umi_ui_navigation_stack_registry_clear(UmiUiNavigationEntryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
