/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_capture_rejections.c
 * PURPOSE: Validate output atomicity, stale checkpoint rejection, invalid
 *          token IDs and non-overlapping trace output requirements.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_capture.h"
#include "umicom/ai/training/stacked_projection_trace.h"
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "K/V capture rejection line %d: %s\n", __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig config = {
        260U, 4U, 4U, 8U, 2U, 2U, UINT64_C(9011)
    };
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedKvCapture *capture = NULL;
    UmiAiTrainingStackedKvCaptureInfo before = {0}, after = {0};
    size_t prefix[2] = {69U, 70U};
    float result[260] = {0};
    CHECK(UmiAiTrainingStackedDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvCaptureCreate(model, &capture) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvCapturePrefill(capture, prefix, 2U,
          result, 260U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvCaptureInspect(capture, &before) == UMI_STATUS_OK);
    result[0] = 54321.0F;
    CHECK(UmiAiTrainingStackedKvCaptureAppend(capture, 260U,
          result, 260U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result[0] == 54321.0F);
    CHECK(UmiAiTrainingStackedKvCaptureAppend(capture, 71U,
          result, 3U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result[0] == 54321.0F);
    CHECK(UmiAiTrainingStackedKvCaptureInspect(capture, &after) == UMI_STATUS_OK);
    CHECK(after.stored.acceptedTokenLayerRows == before.stored.acceptedTokenLayerRows);
    CHECK(after.completeReferenceEvaluations == before.completeReferenceEvaluations);
    {
        const size_t fullContext[4] = {69U, 70U, 71U, 72U};
        float same[2U * 4U * 4U] = {0};
        float logits[4U * 260U] = {0};
        CHECK(UmiAiTrainingStackedDecoderTraceProjections(model,
            fullContext, 4U, same, 32U, same, 32U,
            logits, 1040U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(same[0] == 0.0F);
    }
    {
        const size_t fullContext[4] = {69U, 70U, 71U, 72U};
        const size_t target[4] = {70U, 71U, 72U, 73U};
        const uint8_t mask[4] = {1U, 1U, 1U, 1U};
        UmiAiTrainingStackedStep step = {0};
        /* A changed borrowed checkpoint invalidates the old K/V owner. */
        CHECK(UmiAiTrainingStackedDecoderTrainSelected(model, fullContext,
              target, mask, 4U, 0.0001F, &step) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingStackedKvCaptureAppend(capture, 71U,
              result, 260U) == UMI_STATUS_INVALID_STATE);
        CHECK(result[0] == 54321.0F);
    }
    UmiAiTrainingStackedKvCaptureDestroy(capture);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("K/V source identity, bounds and rejection atomicity passed");
    return 0;
}
