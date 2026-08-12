/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/navigation_stack.h
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

typedef struct UmiUiNavigationEntryRegistry UmiUiNavigationEntryRegistry;

UmiStatus umi_ui_navigation_stack_registry_create(UmiUiNavigationEntryRegistry **out_registry);
void umi_ui_navigation_stack_registry_destroy(UmiUiNavigationEntryRegistry *registry);
UmiStatus umi_ui_navigation_stack_registry_upsert(UmiUiNavigationEntryRegistry *registry, const UmiUiNavigationEntrySnapshot *item);
UmiStatus umi_ui_navigation_stack_registry_remove(UmiUiNavigationEntryRegistry *registry, const char *id);
UmiStatus umi_ui_navigation_stack_registry_find(const UmiUiNavigationEntryRegistry *registry, const char *id, UmiUiNavigationEntrySnapshot *out_item);
UmiStatus umi_ui_navigation_stack_registry_at(const UmiUiNavigationEntryRegistry *registry, size_t index, UmiUiNavigationEntrySnapshot *out_item);
size_t umi_ui_navigation_stack_registry_count(const UmiUiNavigationEntryRegistry *registry);
uint64_t umi_ui_navigation_stack_registry_revision(const UmiUiNavigationEntryRegistry *registry);
void umi_ui_navigation_stack_registry_clear(UmiUiNavigationEntryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
