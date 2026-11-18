/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/sample_order.c
 *
 * PURPOSE:
 *   Publish a complete row permutation only after private construction succeeds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/sample_order.h"
#include "umicom/ai/training/initialisation.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiAiTrainingSampleOrder(size_t rowCount, uint64_t seed,
    uint32_t *outOrder, size_t capacity)
{
    if (outOrder == NULL || rowCount == 0U ||
        rowCount > UMI_AI_TRAINING_MAX_ORDER_ROWS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < rowCount) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Row IDs are bounded independently of pointer width. The private array
     * makes failure atomic without borrowing dataset storage or allocating
     * another copy of its text/tensors. Never choose swap indices with a float
     * or rand()%bound: the shared integer sampler owns rejection bias rules. */
    uint32_t *candidate = malloc(rowCount * sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < rowCount; ++i) candidate[i] = (uint32_t)i;
    UmiAiTrainingRandomState random;
    UmiStatus status = UmiAiTrainingRandomSeed(&random, seed);
    for (size_t remaining = rowCount; status == UMI_STATUS_OK &&
        remaining > 1U; --remaining) {
        uint64_t selected = 0U;
        status = UmiAiTrainingRandomIndex(&random, (uint64_t)remaining, &selected);
        if (status != UMI_STATUS_OK) break;
        uint32_t saved = candidate[remaining - 1U];
        candidate[remaining - 1U] = candidate[(size_t)selected];
        candidate[(size_t)selected] = saved;
    }
    if (status == UMI_STATUS_OK)
        memcpy(outOrder, candidate, rowCount * sizeof(*candidate));
    free(candidate);
    return status;
}
