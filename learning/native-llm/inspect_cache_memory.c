/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-llm/inspect_cache_memory.c
 * PURPOSE: Inspect exactly reserved heap bytes for two native decoder shapes.
 *          No alternative model, training algorithm or interpreter is used.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/inference_profile.h"
#include "umicom/ai/training/kv_cache.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

static int Show(const UmiAiTrainingDecoderConfig *configuration)
{
    UmiAiTrainingInferenceFootprint footprint;
    UmiAiTrainingDecoder *decoder = NULL;
    UmiAiTrainingKvCache *cache = NULL;
    size_t allocated = 0U;
    UmiStatus status = UmiAiTrainingInferenceEstimate(configuration,
                                                        &footprint);
    if (status != UMI_STATUS_OK) return 1;
    status = UmiAiTrainingDecoderCreate(configuration, &decoder);
    if (status != UMI_STATUS_OK) return 1;
    status = UmiAiTrainingKvCacheCreate(decoder, &cache);
    if (status != UMI_STATUS_OK) {
        UmiAiTrainingDecoderDestroy(decoder);
        return 1;
    }
    status = UmiAiTrainingKvCacheAllocatedBytes(cache, &allocated);
    if (status != UMI_STATUS_OK || allocated != footprint.cacheAllocationBytes) {
        UmiAiTrainingKvCacheDestroy(cache);
        UmiAiTrainingDecoderDestroy(decoder);
        return 1;
    }
    printf("vocabulary=%u context=%u channels=%u: model=%zu "
           "cache=%zu key_value_payload=%zu bytes\n",
           (unsigned)configuration->vocabularySize,
           (unsigned)configuration->contextLength,
           (unsigned)configuration->channels,
           footprint.logicalParameterBytes,
           allocated, footprint.keyValuePayloadBytes);
    UmiAiTrainingKvCacheDestroy(cache);
    UmiAiTrainingDecoderDestroy(decoder);
    return 0;
}

int main(void)
{
    UmiAiTrainingDecoderConfig small = {
        17U, 6U, 8U, 16U, 2U, UINT64_C(42)
    };
    UmiAiTrainingDecoderConfig larger = {
        260U, 8U, 8U, 16U, 2U, UINT64_C(42)
    };
    size_t maximum = 0U;
    if (UmiAiTrainingKvCacheAllocationSize(&maximum) != UMI_STATUS_OK)
        return 1;
    printf("Conservative cache reservation upper bound: %zu bytes\n", maximum);
    if (Show(&small) != 0 || Show(&larger) != 0) return 1;
    puts("A smaller allocation is not evidence of lower runtime or higher "
         "model accuracy. Heap allocator metadata and stack staging are excluded.");
    return 0;
}
