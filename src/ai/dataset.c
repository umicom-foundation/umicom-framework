/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/dataset.c
 *
 * PURPOSE:
 *   Maintain bounded evaluation dataset identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Stable item IDs let the actual prompt/answer evidence live in the Data Server while the dataset keeps a compact repeatable membership list.
 */

#include "umicom/ai/dataset.h"
#include <stddef.h>

#include <stdio.h>
#include <string.h>

/*
 * Initialise ai dataset from caller-provided values so later operations receive a known
 * state.
 */
void umi_ai_dataset_init(UmiAiDataset *dataset, const char *dataset_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dataset != NULL) {
        (void)memset(dataset, 0, sizeof(*dataset));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (dataset_id != NULL) {
            (void)snprintf(dataset->dataset_id, sizeof(dataset->dataset_id), "%s", dataset_id);
        }
    }
}

/* Add ai dataset only after its inputs and available capacity have been checked. */
UmiStatus umi_ai_dataset_add(UmiAiDataset *dataset, const char *item_id)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dataset == NULL || item_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (dataset->count >= UMI_AI_DATASET_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(dataset->item_ids[dataset->count],
                       sizeof(dataset->item_ids[dataset->count]), "%s", item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(dataset->item_ids[dataset->count])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++dataset->count;
    return UMI_STATUS_OK;
}
