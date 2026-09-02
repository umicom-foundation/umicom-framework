/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/checkpoint_store.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/checkpoint_store.h"

#include <string.h>

/*
 * Initialise application production checkpoint store from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_production_checkpoint_store_init(
    UmiApplicationProductionCheckpointStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store != NULL) {
        (void)memset(store, 0, sizeof(*store));
        store->next_sequence = 1U;
    }
}

/*
 * Provide the application production checkpoint store put operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_checkpoint_store_put(
    UmiApplicationProductionCheckpointStore *store,
    const UmiApplicationWorkspaceRuntime *runtime, const char *reason,
    int clean_shutdown)
{
    UmiApplicationProductionWorkspaceCheckpoint checkpoint;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || runtime == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_workspace_checkpoint_capture(
        runtime, store->next_sequence, reason, clean_shutdown, &checkpoint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the application production checkpoint store latest operation used by this module
 * and its client applications.
 */
const UmiApplicationProductionWorkspaceCheckpoint *
umi_application_production_checkpoint_store_latest(
    const UmiApplicationProductionCheckpointStore *store,
    const char *application_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || application_id == NULL) return NULL;
    index = store->count;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index > 0U) {
        index -= 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->entries[index].session.application_id,
                   application_id) == 0)
            return &store->entries[index];
    }
    return NULL;
}

