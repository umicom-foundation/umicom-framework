/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_inference_footprint.c
 * PURPOSE: Verify model-shape and cache-allocation accounting without
 *          claiming operating-system resident memory measurements.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/inference_profile.h"
#include "umicom/ai/training/kv_cache.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {17U, 6U, 8U, 16U, 2U, UINT64_C(42)};
    UmiAiTrainingInferenceFootprint result;
    UmiAiTrainingInferenceFootprint previous;
    size_t expectedCacheBytes = 0U;
    size_t compactCacheBytes = 0U;
    size_t preserved = 103U;

    CHECK(UmiAiTrainingInferenceEstimate(&config, &result) == UMI_STATUS_OK);
    /* 15 parameter matrices; every matrix dimension is derived from the
     * canonical one-block decoder architecture. */
    const size_t expectedParameters =
        (17U * 8U) + (6U * 8U) + (4U * 8U) + (4U * 8U * 8U) +
        (8U * 16U) + 16U + (16U * 8U) + 8U + (8U * 17U);
    CHECK(result.parameterTensors == 15U);
    CHECK(result.parameterElements == expectedParameters);
    CHECK(result.logicalParameterBytes == expectedParameters * sizeof(float));
    CHECK(result.keyValuePayloadBytes == 2U * 6U * 8U * sizeof(float));
    CHECK(result.fullContextLogitsBytes == 6U * 17U * sizeof(float));
    CHECK(UmiAiTrainingKvCacheAllocationSize(&expectedCacheBytes) == UMI_STATUS_OK);
    CHECK(expectedCacheBytes > result.logicalParameterBytes);
    /* The previous assertion compared a fixed allocation with a fixed
     * estimated footprint. Retain it for engineering history: exact storage
     * now varies with validated decoder shapes. */
#if 0
    CHECK(result.cacheAllocationBytes == expectedCacheBytes);
#endif
    CHECK(UmiAiTrainingKvCacheAllocationForConfig(&config,
               &compactCacheBytes) == UMI_STATUS_OK);
    CHECK(result.cacheAllocationBytes == compactCacheBytes);
    CHECK(compactCacheBytes < expectedCacheBytes);

    memcpy(&previous, &result, sizeof(previous));
    config.attentionHeads = 3U; /* channels must be divisible by heads */
    CHECK(UmiAiTrainingInferenceEstimate(&config, &result) != UMI_STATUS_OK);
    CHECK(memcmp(&previous, &result, sizeof(result)) == 0);
    CHECK(UmiAiTrainingInferenceEstimate(NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingInferenceEstimate(&config, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvCacheAllocationSize(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(preserved == 103U);
    puts("inference memory budget and parameter shapes: PASS");
    return 0;
}
