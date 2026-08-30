/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/history_entry.h
 *
 * PURPOSE:
 *   Define a provider-neutral source-control workspace record above the low-level VCS adapter boundary.
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
#ifndef UMICOM_SOURCE_CONTROL_HISTORY_ENTRY_H
#define UMICOM_SOURCE_CONTROL_HISTORY_ENTRY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_HISTORY_ENTRY_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_HISTORY_ENTRY_API_VERSION 1U

typedef struct UmiSourceControlHistoryEntrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char revision_id[128];
    char summary[512];
    char author[256];
    uint64_t timestamp;
    int32_t order;
    uint64_t revision;
} UmiSourceControlHistoryEntrySnapshot;

typedef struct UmiSourceControlHistoryEntryRegistry UmiSourceControlHistoryEntryRegistry;

UmiStatus umi_source_control_history_entry_registry_create(UmiSourceControlHistoryEntryRegistry **out_registry);
void umi_source_control_history_entry_registry_destroy(UmiSourceControlHistoryEntryRegistry *registry);
UmiStatus umi_source_control_history_entry_registry_upsert(UmiSourceControlHistoryEntryRegistry *registry, const UmiSourceControlHistoryEntrySnapshot *item);
UmiStatus umi_source_control_history_entry_registry_remove(UmiSourceControlHistoryEntryRegistry *registry, const char *id);
UmiStatus umi_source_control_history_entry_registry_find(const UmiSourceControlHistoryEntryRegistry *registry, const char *id, UmiSourceControlHistoryEntrySnapshot *out_item);
UmiStatus umi_source_control_history_entry_registry_at(const UmiSourceControlHistoryEntryRegistry *registry, size_t index, UmiSourceControlHistoryEntrySnapshot *out_item);
size_t umi_source_control_history_entry_registry_count(const UmiSourceControlHistoryEntryRegistry *registry);
uint64_t umi_source_control_history_entry_registry_revision(const UmiSourceControlHistoryEntryRegistry *registry);
void umi_source_control_history_entry_registry_clear(UmiSourceControlHistoryEntryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
