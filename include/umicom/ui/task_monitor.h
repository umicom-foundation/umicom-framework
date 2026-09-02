/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/task_monitor.h
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
#ifndef UMICOM_UI_TASK_MONITOR_H
#define UMICOM_UI_TASK_MONITOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_TASK_MONITOR_CAPACITY 4096U
#define UMI_UI_TASK_MONITOR_API_VERSION 1U

/**
 * Represent the ui task monitor snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiTaskMonitorSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char label[256];
    char group[128];
    char detail[512];
    uint64_t started_at;
    uint64_t finished_at;
    int state;
    int background;
    uint64_t revision;
} UmiUiTaskMonitorSnapshot;

/**
 * Represent the ui task monitor registry data shared with callers of this public contract.
 */
typedef struct UmiUiTaskMonitorRegistry UmiUiTaskMonitorRegistry;

/**
 * Initialise ui task monitor registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_task_monitor_registry_create(UmiUiTaskMonitorRegistry **out_registry);
/**
 * Release or reset state held by ui task monitor registry so the same storage can be
 * reused safely.
 */
void umi_ui_task_monitor_registry_destroy(UmiUiTaskMonitorRegistry *registry);
/**
 * Provide the ui task monitor registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_task_monitor_registry_upsert(UmiUiTaskMonitorRegistry *registry, const UmiUiTaskMonitorSnapshot *item);
/**
 * Remove ui task monitor registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_task_monitor_registry_remove(UmiUiTaskMonitorRegistry *registry, const char *id);
/**
 * Find ui task monitor registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_task_monitor_registry_find(const UmiUiTaskMonitorRegistry *registry, const char *id, UmiUiTaskMonitorSnapshot *out_item);
/**
 * Find ui task monitor registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_task_monitor_registry_at(const UmiUiTaskMonitorRegistry *registry, size_t index, UmiUiTaskMonitorSnapshot *out_item);
/**
 * Return the number of records represented by ui task monitor registry without changing
 * their state.
 */
size_t umi_ui_task_monitor_registry_count(const UmiUiTaskMonitorRegistry *registry);
/**
 * Provide the ui task monitor registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_task_monitor_registry_revision(const UmiUiTaskMonitorRegistry *registry);
/**
 * Release or reset state held by ui task monitor registry so the same storage can be
 * reused safely.
 */
void umi_ui_task_monitor_registry_clear(UmiUiTaskMonitorRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
