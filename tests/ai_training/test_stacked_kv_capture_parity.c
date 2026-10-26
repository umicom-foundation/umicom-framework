/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_capture_parity.c
 * PURPOSE: Exercise canonical model logits, tapped K/V, growing-prefix row
 *          invariance and mandatory whole-window learned-position rebuilding.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_capture.h"
#include "umicom/ai/training/stacked_projection_trace.h"
#include <math.h>
#include <stdio.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "real K/V parity line %d: %s\n", __LINE__, #condition); \
    return 1; } } while (0)

static int EqualArrays(const float *left, const float *right,
                       size_t count, float tolerance)
{
    for (size_t i = 0U; i < count; ++i)
        if (!isfinite(left[i]) || !isfinite(right[i]) ||
            fabsf(left[i] - right[i]) > tolerance) return 0;
    return 1;
}

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig config = {
        260U, 4U, 4U, 8U, 2U, 2U, UINT64_C(20041)
    };
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedKvCapture *capture = NULL;
    UmiAiTrainingStackedKvCaptureInfo info = {0};
    float original[4U * 260U] = {0};
    float tapped[4U * 260U] = {0};
    float keys[2U * 4U * 4U] = {0};
    float values[2U * 4U * 4U] = {0};
    float last[260U] = {0};
    const size_t context[4] = {69U, 70U, 71U, 72U};
    const size_t prefix[2] = {69U, 70U};
    const size_t shifted[4] = {70U, 71U, 72U, 73U};

    CHECK(UmiAiTrainingStackedDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderTraceProjections(model, context, 4U,
              keys, 32U, values, 32U, tapped, 1040U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderLogits(model, context, 4U,
              original, 1040U) == UMI_STATUS_OK);
    CHECK(EqualArrays(original, tapped, 1040U, 0.00004F));
    CHECK(UmiAiTrainingStackedKvCaptureCreate(model, &capture) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvCapturePrefill(capture, prefix, 2U,
              last, 260U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvCaptureInspect(capture, &info) == UMI_STATUS_OK);
    CHECK(info.stored.retainedTokens == 2U &&
          info.stored.acceptedTokenLayerRows == 4U &&
          info.actuallyEvaluatedTokenLayerRows == 8U);
    CHECK(UmiAiTrainingStackedKvCaptureAppend(capture, 71U,
              last, 260U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvCaptureAppend(capture, 72U,
              last, 260U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvCaptureInspect(capture, &info) == UMI_STATUS_OK);
    CHECK(info.stored.retainedTokens == 4U &&
          info.stored.acceptedTokenLayerRows == 8U &&
          info.actuallyEvaluatedTokenLayerRows == 24U &&
          info.maximumPreviousRowDifference <= 0.00004F);
    CHECK(EqualArrays(original + 3U * 260U, last, 260U, 0.00004F));
    CHECK(UmiAiTrainingStackedKvCaptureAppend(capture, 73U,
              last, 260U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvCaptureInspect(capture, &info) == UMI_STATUS_OK);
    CHECK(info.stored.fullWindowRebuilds == 1U &&
          info.stored.acceptedTokenLayerRows == 16U &&
          info.actuallyEvaluatedTokenLayerRows == 32U);
    CHECK(UmiAiTrainingStackedDecoderLogits(model, shifted, 4U,
              original, 1040U) == UMI_STATUS_OK);
    CHECK(EqualArrays(original + 3U * 260U, last, 260U, 0.00004F));
    UmiAiTrainingStackedKvCaptureDestroy(capture);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("Canonical stacked K/V observation and rollover parity passed");
    return 0;
}
