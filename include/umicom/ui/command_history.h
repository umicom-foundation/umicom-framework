/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/command_history.h
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

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_UI_COMMAND_HISTORY_H
#define UMICOM_UI_COMMAND_HISTORY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_COMMAND_HISTORY_CAPACITY 4096U
#define UMI_UI_COMMAND_HISTORY_API_VERSION 1U

typedef struct UmiUiCommandHistorySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char command_id[128];
    char argument[1024];
    char source[128];
    uint64_t executed_at;
    int outcome;
    uint64_t revision;
} UmiUiCommandHistorySnapshot;

typedef struct UmiUiCommandHistoryRegistry UmiUiCommandHistoryRegistry;

UmiStatus umi_ui_command_history_registry_create(UmiUiCommandHistoryRegistry **out_registry);
void umi_ui_command_history_registry_destroy(UmiUiCommandHistoryRegistry *registry);
UmiStatus umi_ui_command_history_registry_upsert(UmiUiCommandHistoryRegistry *registry, const UmiUiCommandHistorySnapshot *item);
UmiStatus umi_ui_command_history_registry_remove(UmiUiCommandHistoryRegistry *registry, const char *id);
UmiStatus umi_ui_command_history_registry_find(const UmiUiCommandHistoryRegistry *registry, const char *id, UmiUiCommandHistorySnapshot *out_item);
UmiStatus umi_ui_command_history_registry_at(const UmiUiCommandHistoryRegistry *registry, size_t index, UmiUiCommandHistorySnapshot *out_item);
size_t umi_ui_command_history_registry_count(const UmiUiCommandHistoryRegistry *registry);
uint64_t umi_ui_command_history_registry_revision(const UmiUiCommandHistoryRegistry *registry);
void umi_ui_command_history_registry_clear(UmiUiCommandHistoryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
