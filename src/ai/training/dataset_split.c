/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/dataset_split.c
 *
 * PURPOSE:
 *   Reproducibly divide whole sequence IDs into disjoint training/evaluation
 *   groups. Actual corpus bytes are never moved, copied or reclassified here.
 *
 * NOTES:
 *   Deterministic SplitMix64 state from the existing Framework initialiser
 *   is used for bounded Fisher-Yates swaps. This is an educational PRNG, not
 *   a cryptographically secure or statistically stratified sampler.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/dataset_split.h"
#include "umicom/ai/training/initialisation.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiAiTrainingSplitSequences(size_t sequenceCount,
                                     size_t evaluationCount,
                                     uint64_t seed,
                                     size_t *outOrder,
                                     size_t capacity)
{
    UmiAiTrainingRandomState rng;
    size_t *order;
    UmiStatus status;
    if (outOrder == NULL || sequenceCount < 2U ||
        sequenceCount > 4096U || evaluationCount == 0U ||
        evaluationCount >= sequenceCount) return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < sequenceCount) return UMI_STATUS_CAPACITY_EXCEEDED;
    order = malloc(sequenceCount * sizeof(*order));
    if (order == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingRandomSeed(&rng, seed);
    if (status != UMI_STATUS_OK) { free(order); return status; }
    for (size_t i = 0U; i < sequenceCount; ++i) order[i] = i;
    /* Uniform-to-index maps are straightforward for teaching. For production
     * unbiased integer sampling, use rejection sampling on raw RNG words. */
    for (size_t remaining = sequenceCount; remaining > 1U; --remaining) {
        float sample = 0.0F;
        status = UmiAiTrainingRandomUniform(&rng, 0.0F, 1.0F, &sample);
        if (status != UMI_STATUS_OK) { free(order); return status; }
        size_t position = (size_t)(sample * (float)remaining);
        if (position >= remaining) position = remaining - 1U;
        size_t tmp = order[position];
        order[position] = order[remaining - 1U];
        order[remaining - 1U] = tmp;
    }
    memcpy(outOrder, order, sequenceCount * sizeof(*order));
    free(order);
    return UMI_STATUS_OK;
}
