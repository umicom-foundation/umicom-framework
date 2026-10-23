/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_decoder_clipped.c
 * PURPOSE: Verify decoder-wide clipping applies after microbatch reduction.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_decoder_clipped.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/gradient_accumulation.h"
#include "umicom/ai/training/optimizer.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {12U, 3U, 4U, 8U, 2U, 42U};
    UmiAiTrainingAdamWConfig settings;
    UmiAiTrainingDecoder *direct = NULL, *clipped = NULL;
    UmiAiTrainingAdamW *first = NULL, *second = NULL;
    const size_t input[] = {1U, 2U, 3U, 4U, 5U, 6U};
    const size_t target[] = {2U, 3U, 4U, 5U, 6U, 7U};
    float ordinaryLoss = 0.0F, controlledLoss = 0.0F;
    UmiAiTrainingGradientClipReport report = {0};
    uint8_t reference[131072], result[131072];
    size_t n1 = 0U, n2 = 0U;
    uint64_t steps = 0U;
    UmiAiTrainingDecoderConfig inspected;
    CHECK(UmiAiTrainingAdamWDefaults(&settings) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&config, &direct) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&config, &clipped) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(direct, &settings, &first)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(clipped, &settings, &second)
          == UMI_STATUS_OK);

    CHECK(UmiAiTrainingDecoderTrainAdamWAccumulated(direct, first,
          input, target, 6U, 1U, 0.002F, &ordinaryLoss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainAdamWAccumulatedClipped(clipped, second,
          input, target, 6U, 1U, 0.002F, 1e30F,
          &controlledLoss, &report) == UMI_STATUS_OK);
    CHECK(report.wasClipped == 0 && report.appliedScale == 1.0);
    CHECK(report.globalNorm >= 0.0 && report.gradientElements > 0U);
    CHECK(ordinaryLoss == controlledLoss);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(direct, reference,
          sizeof(reference), &n1) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(clipped, result,
          sizeof(result), &n2) == UMI_STATUS_OK);
    CHECK(n1 == n2 && memcmp(reference, result, n1) == 0);

    UmiAiTrainingGradientClipReport prior = report;
    float previousLoss = controlledLoss;
    CHECK(UmiAiTrainingDecoderTrainAdamWAccumulatedClipped(clipped, second,
          input, target, 6U, 1U, 0.002F, 0.0F,
          &controlledLoss, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&prior, &report, sizeof(report)) == 0);
    CHECK(controlledLoss == previousLoss);
    CHECK(UmiAiTrainingDecoderInspect(clipped, &inspected, &steps)
          == UMI_STATUS_OK && steps == 1U);

    /* A second update with a strict threshold must actually clip the model's
     * full weighted gradient, not only the separate optimiser fixture. */
    CHECK(UmiAiTrainingDecoderTrainAdamWAccumulatedClipped(clipped, second,
          input, target, 6U, 1U, 0.002F, 0.01F,
          &controlledLoss, &report) == UMI_STATUS_OK);
    CHECK(report.wasClipped == 1 && report.appliedScale < 1.0);
    CHECK(fabs(report.globalNorm * report.appliedScale - 0.01) < 1e-9);
    CHECK(UmiAiTrainingDecoderInspect(clipped, &inspected, &steps)
          == UMI_STATUS_OK && steps == 2U);

    UmiAiTrainingAdamWDestroy(second);
    UmiAiTrainingAdamWDestroy(first);
    UmiAiTrainingDecoderDestroy(clipped);
    UmiAiTrainingDecoderDestroy(direct);
    return 0;
}
