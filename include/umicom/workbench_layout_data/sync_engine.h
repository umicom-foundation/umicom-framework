/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/sync_engine.h
 *
 * PURPOSE:
 *   Execute planned layout synchronisation through store ports, explicit conflicts and durable per-peer cursors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_SYNC_ENGINE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_SYNC_ENGINE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/workbench_layout/store.h"
#include "umicom/workbench_layout_data/conflict_store.h"
#include "umicom/workbench_layout_data/sync_cursor.h"
#include "umicom/workbench_layout_data/sync_plan.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutSyncEndpoint {
    uint32_t structure_size;
    char replica_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    UmiWorkbenchLayoutStoreAdapter store;
    const UmiWorkbenchLayoutChange *changes;
    size_t change_count;
} UmiWorkbenchLayoutSyncEndpoint;

typedef struct UmiWorkbenchLayoutSyncPolicy {
    uint32_t structure_size;
    bool continue_after_conflict;
    bool allow_remote_delete;
    bool allow_local_delete;
    bool require_fencing_for_push;
    size_t maximum_operations;
} UmiWorkbenchLayoutSyncPolicy;

typedef struct UmiWorkbenchLayoutSyncResult {
    uint32_t structure_size;
    UmiStatus status;
    size_t planned_count;
    size_t applied_count;
    size_t skipped_count;
    size_t conflict_count;
    size_t failed_count;
    uint64_t last_pulled_sequence;
    uint64_t last_pushed_sequence;
    char failed_layout_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char message[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutSyncResult;

typedef struct UmiWorkbenchLayoutSyncEngine {
    uint32_t structure_size;
    UmiWorkbenchLayoutConflictStoreRepository *conflicts;
    UmiWorkbenchLayoutSyncCursorRepository *cursors;
    UmiWorkbenchLayoutSyncPolicy policy;
    uint64_t revision;
} UmiWorkbenchLayoutSyncEngine;

UmiWorkbenchLayoutSyncPolicy
umi_workbench_layout_sync_policy_default(void);

UmiStatus umi_workbench_layout_sync_engine_init(
    UmiWorkbenchLayoutSyncEngine *engine,
    UmiWorkbenchLayoutConflictStoreRepository *conflicts,
    UmiWorkbenchLayoutSyncCursorRepository *cursors,
    const UmiWorkbenchLayoutSyncPolicy *policy);

UmiStatus umi_workbench_layout_sync_engine_plan(
    UmiWorkbenchLayoutSyncEngine *engine,
    const UmiWorkbenchLayoutSyncEndpoint *local,
    const UmiWorkbenchLayoutSyncEndpoint *remote,
    const UmiWorkbenchLayoutSyncCursor *cursor,
    UmiWorkbenchLayoutDataSyncDirection direction,
    uint64_t created_at_ms,
    UmiWorkbenchLayoutSyncPlan *out_plan);

UmiStatus umi_workbench_layout_sync_engine_apply(
    UmiWorkbenchLayoutSyncEngine *engine,
    const UmiWorkbenchLayoutSyncPlan *plan,
    UmiWorkbenchLayoutSyncEndpoint *local,
    UmiWorkbenchLayoutSyncEndpoint *remote,
    const char *actor_id,
    uint64_t now_ms,
    UmiWorkbenchLayoutSyncResult *out_result);

UmiStatus umi_workbench_layout_sync_engine_synchronise(
    UmiWorkbenchLayoutSyncEngine *engine,
    UmiWorkbenchLayoutSyncEndpoint *local,
    UmiWorkbenchLayoutSyncEndpoint *remote,
    UmiWorkbenchLayoutSyncCursor *cursor,
    UmiWorkbenchLayoutDataSyncDirection direction,
    const char *actor_id,
    uint64_t now_ms,
    UmiWorkbenchLayoutSyncResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
