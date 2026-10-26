/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_capture_reset.c
 * PURPOSE: Confirm deterministic repeatable projection capture and complete
 *          cleanup of prompt-owned cache state without modifying the decoder.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_capture.h"
#include <math.h>
#include <stdio.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "K/V reset line %d: %s\n", __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig config = {
        260U, 4U, 4U, 8U, 2U, 2U, UINT64_C(19011)
    };
    const size_t prefix[3] = {69U, 70U, 71U};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedKvCapture *capture = NULL;
    float initial[260] = {0}, later[260] = {0};
    float key[4] = {0}, value[4] = {0};
    UmiAiTrainingStackedKvCaptureInfo report = {0};
    CHECK(UmiAiTrainingStackedDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvCaptureCreate(model, &capture) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvCapturePrefill(capture, prefix, 3U,
          initial, 260U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvCaptureRead(capture, 1U, 2U,
          key, 4U, value, 4U) == UMI_STATUS_OK);
    for (size_t j = 0U; j < 4U; ++j)
        CHECK(isfinite(key[j]) && isfinite(value[j]));
    CHECK(UmiAiTrainingStackedKvCaptureReset(capture) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedKvCaptureInspect(capture, &report) == UMI_STATUS_OK);
    CHECK(report.stored.retainedTokens == 0U &&
          report.stored.acceptedTokenLayerRows == 0U &&
          report.actuallyEvaluatedTokenLayerRows == 0U);
    CHECK(UmiAiTrainingStackedKvCapturePrefill(capture, prefix, 3U,
          later, 260U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 260U; ++i)
        CHECK(isfinite(initial[i]) && isfinite(later[i]) &&
              fabsf(initial[i] - later[i]) <= 0.00004F);
    UmiAiTrainingStackedKvCaptureDestroy(capture);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("K/V capture reset and repeatable canonical logits passed");
    return 0;
}
