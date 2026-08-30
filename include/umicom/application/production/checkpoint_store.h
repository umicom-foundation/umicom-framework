/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/checkpoint_store.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_CHECKPOINT_STORE_H
#define UMICOM_APPLICATION_PRODUCTION_CHECKPOINT_STORE_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/workspace_checkpoint.h"

typedef struct UmiApplicationProductionCheckpointStore {
    UmiApplicationProductionWorkspaceCheckpoint
        entries[UMI_APPLICATION_PRODUCTION_MAX_CHECKPOINTS];
    size_t count;
    uint64_t next_sequence;
} UmiApplicationProductionCheckpointStore;

void umi_application_production_checkpoint_store_init(
    UmiApplicationProductionCheckpointStore *store);
UmiStatus umi_application_production_checkpoint_store_put(
    UmiApplicationProductionCheckpointStore *store,
    const UmiApplicationWorkspaceRuntime *runtime, const char *reason,
    int clean_shutdown);
const UmiApplicationProductionWorkspaceCheckpoint *
umi_application_production_checkpoint_store_latest(
    const UmiApplicationProductionCheckpointStore *store,
    const char *application_id);

#ifdef __cplusplus
}
#endif
#endif
