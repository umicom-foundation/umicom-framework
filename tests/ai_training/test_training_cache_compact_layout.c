/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_cache_compact_layout.c
 * PURPOSE: Verify shape-derived allocation, owned bytes and upper bound.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/inference_profile.h"
#include "umicom/ai/training/kv_cache.h"
#include <stdint.h>
#include <stdio.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); \
    return 1; } } while (0)

static int Verify(const UmiAiTrainingDecoderConfig *config, size_t *outBytes)
{
    UmiAiTrainingInferenceFootprint footprint = {0};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingKvCache *cache = NULL;
    size_t planned = 0U;
    size_t actual = 0U;
    CHECK(UmiAiTrainingInferenceEstimate(config, &footprint) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheAllocationForConfig(config, &planned) == UMI_STATUS_OK);
    CHECK(footprint.cacheAllocationBytes == planned);
    CHECK(planned > footprint.logicalParameterBytes +
                    footprint.keyValuePayloadBytes);
    CHECK(UmiAiTrainingDecoderCreate(config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheCreate(model, &cache) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheAllocatedBytes(cache, &actual) == UMI_STATUS_OK);
    CHECK(actual == planned);
    *outBytes = actual;
    UmiAiTrainingKvCacheDestroy(cache);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}

int main(void)
{
    const UmiAiTrainingDecoderConfig small = {
        17U, 6U, 8U, 16U, 2U, UINT64_C(42)
    };
    const UmiAiTrainingDecoderConfig larger = {
        260U, 8U, 8U, 16U, 2U, UINT64_C(42)
    };
    UmiAiTrainingDecoderConfig invalid = small;
    size_t compactSmall = 0U;
    size_t compactLarge = 0U;
    size_t upperBound = 0U;
    size_t unchanged = 1234567U;
    CHECK(Verify(&small, &compactSmall) == 0);
    CHECK(Verify(&larger, &compactLarge) == 0);
    CHECK(compactLarge > compactSmall);
    CHECK(UmiAiTrainingKvCacheAllocationSize(&upperBound) == UMI_STATUS_OK);
    CHECK(upperBound >= compactLarge);
    CHECK(upperBound >= compactSmall);
    CHECK(compactSmall < upperBound / 2U);
    CHECK(compactLarge < upperBound / 2U);

    invalid.attentionHeads = 3U;
    CHECK(UmiAiTrainingKvCacheAllocationForConfig(&invalid, &unchanged) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(unchanged == 1234567U);
    CHECK(UmiAiTrainingKvCacheAllocationForConfig(NULL, &unchanged) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(unchanged == 1234567U);
    CHECK(UmiAiTrainingKvCacheAllocationForConfig(&small, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvCacheAllocatedBytes(NULL, &unchanged) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(unchanged == 1234567U);
    CHECK(UmiAiTrainingKvCacheAllocationSize(NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    printf("compact cache bytes small=%zu large=%zu upper=%zu\n",
           compactSmall, compactLarge, upperBound);
    return 0;
}
