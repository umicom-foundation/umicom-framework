/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/store.c
 *
 * PURPOSE:
 *   Keep an in-memory bounded store of generations for deterministic tests and simple local release state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The store is a reference implementation that can later be replaced by the Data Server without changing callers.
 */

#include "umicom/delivery/store.h"
#include <string.h>
/*
 * Initialise generation store from caller-provided values so later operations receive a
 * known state.
 */
void umi_generation_store_init(UmiGenerationStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store != NULL) (void)memset(store, 0, sizeof(*store));
}
/* Add generation store only after its inputs and available capacity have been checked. */
UmiStatus umi_generation_store_add(UmiGenerationStore *store,
                                   const UmiGeneration *generation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || generation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= UMI_DELIVERY_MAX_RELEASES) return UMI_STATUS_CAPACITY_EXCEEDED;
    store->generations[store->count++] = *generation;
    return UMI_STATUS_OK;
}
/*
 * Provide the generation store active operation used by this module and its client
 * applications.
 */
const UmiGeneration *umi_generation_store_active(const UmiGenerationStore *store)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < store->count; ++i) {
        /* Apply this operation only while the related capability or state is available. */
        if (store->generations[i].active) return &store->generations[i];
    }
    return NULL;
}
