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

/*
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

/**
 * Represent the source control history entry snapshot data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the source control history entry registry data shared with callers of this
 * public contract.
 */
typedef struct UmiSourceControlHistoryEntryRegistry UmiSourceControlHistoryEntryRegistry;

/**
 * Initialise source control history entry registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_history_entry_registry_create(UmiSourceControlHistoryEntryRegistry **out_registry);
/**
 * Release or reset state held by source control history entry registry so the same storage
 * can be reused safely.
 */
void umi_source_control_history_entry_registry_destroy(UmiSourceControlHistoryEntryRegistry *registry);
/**
 * Provide the source control history entry registry upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_source_control_history_entry_registry_upsert(UmiSourceControlHistoryEntryRegistry *registry, const UmiSourceControlHistoryEntrySnapshot *item);
/**
 * Remove source control history entry registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_source_control_history_entry_registry_remove(UmiSourceControlHistoryEntryRegistry *registry, const char *id);
/**
 * Find source control history entry registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_source_control_history_entry_registry_find(const UmiSourceControlHistoryEntryRegistry *registry, const char *id, UmiSourceControlHistoryEntrySnapshot *out_item);
/**
 * Find source control history entry registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_source_control_history_entry_registry_at(const UmiSourceControlHistoryEntryRegistry *registry, size_t index, UmiSourceControlHistoryEntrySnapshot *out_item);
/**
 * Return the number of records represented by source control history entry registry
 * without changing their state.
 */
size_t umi_source_control_history_entry_registry_count(const UmiSourceControlHistoryEntryRegistry *registry);
/**
 * Provide the source control history entry registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_source_control_history_entry_registry_revision(const UmiSourceControlHistoryEntryRegistry *registry);
/**
 * Release or reset state held by source control history entry registry so the same storage
 * can be reused safely.
 */
void umi_source_control_history_entry_registry_clear(UmiSourceControlHistoryEntryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
