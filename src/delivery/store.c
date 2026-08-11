/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/store.c
 *
 * PURPOSE:
 *   Keep an in-memory bounded store of generations for deterministic tests and simple local release state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The store is a reference implementation that can later be replaced by the Data Server without changing callers.
 */

#include "umicom/delivery/store.h"
#include <string.h>
void umi_generation_store_init(UmiGenerationStore *store)
{
    if (store != NULL) (void)memset(store, 0, sizeof(*store));
}
UmiStatus umi_generation_store_add(UmiGenerationStore *store,
                                   const UmiGeneration *generation)
{
    if (store == NULL || generation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (store->count >= UMI_DELIVERY_MAX_RELEASES) return UMI_STATUS_CAPACITY_EXCEEDED;
    store->generations[store->count++] = *generation;
    return UMI_STATUS_OK;
}
const UmiGeneration *umi_generation_store_active(const UmiGenerationStore *store)
{
    size_t i;
    if (store == NULL) return NULL;
    for (i = 0U; i < store->count; ++i) {
        if (store->generations[i].active) return &store->generations[i];
    }
    return NULL;
}
