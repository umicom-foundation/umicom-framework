/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_inference_memory_plan.c
 *
 * PURPOSE:
 *   Verify bounded KV memory calculations, admission and atomic rejection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/inference_memory_plan.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiAiTrainingInferenceMemoryRequest r = {
        .layers = 12U, .contextTokens = 2048U, .queryHeads = 16U,
        .keyValueHeads = 4U, .headChannels = 64U,
        .bytesPerScalar = 2U, .sessionCount = 3U,
        .budgetBytes = 50U * 1024U * 1024U
    };
    UmiAiTrainingInferenceMemoryReport out = {0};
    assert(UmiAiTrainingInferenceMemoryPlan(&r, &out) == UMI_STATUS_OK);
    assert(out.scalarsPerSession == 12U * 2048U * 4U * 64U * 2U);
    assert(out.bytesPerSession == 24U * 1024U * 1024U);
    assert(out.totalBytes == 72U * 1024U * 1024U);
    assert(out.fullMultiheadTotalBytes == 288U * 1024U * 1024U);
    assert(out.reductionBytes == 216U * 1024U * 1024U);
    assert(out.admittedSessions == 2U && out.fitsBudget == 0);
    r.budgetBytes = 0U;
    assert(UmiAiTrainingInferenceMemoryPlan(&r, &out) == UMI_STATUS_OK);
    assert(out.fitsBudget == 1 && out.admittedSessions == 3U);
    UmiAiTrainingInferenceMemoryReport prior = out;
    r.queryHeads = 15U;
    assert(UmiAiTrainingInferenceMemoryPlan(&r, &out) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&prior, &out, sizeof(out)) == 0);
    r.queryHeads = 16U;
    r.layers = SIZE_MAX;
    assert(UmiAiTrainingInferenceMemoryPlan(&r, &out) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(&prior, &out, sizeof(out)) == 0);
    puts("KV memory admission checks passed");
    return 0;
}
