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

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_UI_COMMAND_HISTORY_H
#define UMICOM_UI_COMMAND_HISTORY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_COMMAND_HISTORY_CAPACITY 4096U
#define UMI_UI_COMMAND_HISTORY_API_VERSION 1U

/**
 * Represent the ui command history snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the ui command history registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiCommandHistoryRegistry UmiUiCommandHistoryRegistry;

/**
 * Initialise ui command history registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_command_history_registry_create(UmiUiCommandHistoryRegistry **out_registry);
/**
 * Release or reset state held by ui command history registry so the same storage can be
 * reused safely.
 */
void umi_ui_command_history_registry_destroy(UmiUiCommandHistoryRegistry *registry);
/**
 * Provide the ui command history registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_command_history_registry_upsert(UmiUiCommandHistoryRegistry *registry, const UmiUiCommandHistorySnapshot *item);
/**
 * Remove ui command history registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_command_history_registry_remove(UmiUiCommandHistoryRegistry *registry, const char *id);
/**
 * Find ui command history registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_command_history_registry_find(const UmiUiCommandHistoryRegistry *registry, const char *id, UmiUiCommandHistorySnapshot *out_item);
/**
 * Find ui command history registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_command_history_registry_at(const UmiUiCommandHistoryRegistry *registry, size_t index, UmiUiCommandHistorySnapshot *out_item);
/**
 * Return the number of records represented by ui command history registry without changing
 * their state.
 */
size_t umi_ui_command_history_registry_count(const UmiUiCommandHistoryRegistry *registry);
/**
 * Provide the ui command history registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_command_history_registry_revision(const UmiUiCommandHistoryRegistry *registry);
/**
 * Release or reset state held by ui command history registry so the same storage can be
 * reused safely.
 */
void umi_ui_command_history_registry_clear(UmiUiCommandHistoryRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_ui_command_history_snapshot_validate(const UmiUiCommandHistorySnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_UI_COMMAND_HISTORY_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_ui_command_history_registry_upsert_many(UmiUiCommandHistoryRegistry *registry,
    const UmiUiCommandHistorySnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
