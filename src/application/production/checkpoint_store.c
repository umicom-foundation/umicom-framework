/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/checkpoint_store.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/checkpoint_store.h"

#include <string.h>

void umi_application_production_checkpoint_store_init(
    UmiApplicationProductionCheckpointStore *store)
{
    if (store != NULL) {
        (void)memset(store, 0, sizeof(*store));
        store->next_sequence = 1U;
    }
}

UmiStatus umi_application_production_checkpoint_store_put(
    UmiApplicationProductionCheckpointStore *store,
    const UmiApplicationWorkspaceRuntime *runtime, const char *reason,
    int clean_shutdown)
{
    UmiApplicationProductionWorkspaceCheckpoint checkpoint;
    UmiStatus status;
    if (store == NULL || runtime == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_workspace_checkpoint_capture(
        runtime, store->next_sequence, reason, clean_shutdown, &checkpoint);
    if (status != UMI_STATUS_OK) return status;
    if (store->count == UMI_APPLICATION_PRODUCTION_MAX_CHECKPOINTS) {
        (void)memmove(&store->entries[0], &store->entries[1],
            (store->count - 1U) * sizeof(store->entries[0]));
        store->count -= 1U;
    }
    store->entries[store->count] = checkpoint;
    store->count += 1U;
    store->next_sequence += 1U;
    return UMI_STATUS_OK;
}

const UmiApplicationProductionWorkspaceCheckpoint *
umi_application_production_checkpoint_store_latest(
    const UmiApplicationProductionCheckpointStore *store,
    const char *application_id)
{
    size_t index;
    if (store == NULL || application_id == NULL) return NULL;
    index = store->count;
    while (index > 0U) {
        index -= 1U;
        if (strcmp(store->entries[index].session.application_id,
                   application_id) == 0)
            return &store->entries[index];
    }
    return NULL;
}

