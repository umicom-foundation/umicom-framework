/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_inference_parity.c
 * PURPOSE: Compare entire sampled prefix-to-rollover inference workloads,
 *          including measured work counts and strict logit parity.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/inference_profile.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {17U, 6U, 8U, 16U, 2U, UINT64_C(41)};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingInferenceProfile report = {0};
    UmiAiTrainingInferenceProfile full = {0};
    uint64_t stepsBefore = 999U, stepsAfter = 999U;
    UmiAiTrainingDecoderConfig inspected = {0};
    const size_t stream[] = {4U, 5U, 6U, 9U, 8U, 7U, 10U, 12U, 14U, 16U, 4U};
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderInspect(model, &inspected, &stepsBefore) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingInferenceProfileStream(model, stream, 11U, 2U,
                                                &report) == UMI_STATUS_OK);
    CHECK(report.streamTokens == 11U && report.prefillTokens == 2U);
    CHECK(report.referenceQueries == 10U);
    CHECK(report.referenceComputedRows == 60U);
    CHECK(report.cacheComputedRows == 36U);
    CHECK(report.cacheWindowRebuilds == 5U);
    CHECK(report.retainedTokens == 6U);
    CHECK(report.maximumLogitDeviation < 0.00005);
    CHECK(report.cachedProcessCpuSeconds >= 0.0);
    CHECK(report.referenceProcessCpuSeconds >= 0.0);

    /* With a full prefill, every added token rebases learned positions, so
     * the cache computes just as many rows as complete reference forwards. */
    CHECK(UmiAiTrainingInferenceProfileStream(model, stream, 11U, 6U,
                                                &full) == UMI_STATUS_OK);
    CHECK(full.referenceQueries == 6U);
    CHECK(full.referenceComputedRows == 36U);
    CHECK(full.cacheComputedRows == 36U);
    CHECK(full.cacheWindowRebuilds == 5U);
    CHECK(full.maximumLogitDeviation < 0.00005);
    CHECK(UmiAiTrainingDecoderInspect(model, &inspected, &stepsAfter) == UMI_STATUS_OK);
    CHECK(stepsBefore == 0U && stepsAfter == stepsBefore);
    UmiAiTrainingDecoderDestroy(model);
    puts("prefix, growing context, rolling window and full prefill parity: PASS");
    return 0;
}
