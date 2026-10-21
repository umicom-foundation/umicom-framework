/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_wide_tape.c
 * PURPOSE:
 *   Train through a matrix whose output exceeds the former scalar ceiling;
 *   retain gradient, budget and rewind guarantees in the larger tape.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "wide tape %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    const size_t inputCount = 32U * 8U;
    const size_t weightCount = 8U * 260U;
    const size_t outputCount = 32U * 260U;
    float *input = calloc(inputCount, sizeof(*input));
    float *weights = calloc(weightCount, sizeof(*weights));
    float *targets = calloc(outputCount, sizeof(*targets));
    float *gradient = calloc(weightCount, sizeof(*gradient));
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *x = NULL, *w = NULL, *truth = NULL;
    UmiAiTrainingTensor *prediction = NULL, *loss = NULL;
    UmiAiTrainingTapeMemory before, after;
    size_t checkpoint = 0U;
    float lossValue = 0.0F;
    CHECK(input != NULL && weights != NULL && targets != NULL && gradient != NULL);
    for (size_t i = 0U; i < inputCount; ++i)
        input[i] = (float)(i % 7U) / 8.0F;
    for (size_t i = 0U; i < weightCount; ++i)
        weights[i] = (float)((int)(i % 13U) - 6) / 64.0F;

    CHECK(outputCount > 4096U && outputCount <= UMI_AI_TRAINING_MAX_ELEMENTS);
    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 32U, 8U, input, inputCount,
          0, &x) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 8U, 260U, weights, weightCount,
          1, &w) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 32U, 260U, targets, outputCount,
          0, &truth) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &before) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeCheckpoint(tape, &checkpoint) == UMI_STATUS_OK);
    CHECK(checkpoint == 3U);
    CHECK(UmiAiTrainingMatmul(tape, x, w, &prediction) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingMeanSquaredError(tape, prediction, truth, &loss)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(loss, &lossValue, 1U) == UMI_STATUS_OK);
    CHECK(isfinite(lossValue) && lossValue > 0.0F);
    CHECK(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorGradients(w, gradient, weightCount)
          == UMI_STATUS_OK);
    double magnitude = 0.0;
    for (size_t i = 0U; i < weightCount; ++i) {
        CHECK(isfinite(gradient[i]));
        magnitude += fabs((double)gradient[i]);
    }
    CHECK(magnitude > 0.0);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.currentBytes > before.currentBytes);
    CHECK(after.peakBytes >= after.currentBytes);
    CHECK(UmiAiTrainingSgdStep(tape, w, 0.1F) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeRewind(tape, checkpoint) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.currentBytes == before.currentBytes);
    CHECK(after.tensorCount == 3U);
    CHECK(after.peakBytes > after.currentBytes);
    /* Oversized leaf dimensions are rejected without changing the graph. */
    CHECK(UmiAiTrainingTensorCreate(tape, UMI_AI_TRAINING_MAX_ELEMENTS + 1U,
        1U, weights, 1U, 0, &prediction) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.tensorCount == 3U);

    UmiAiTrainingTapeDestroy(tape);
    free(gradient);
    free(targets);
    free(weights);
    free(input);
    puts("expanded tensor forward/backward, SGD, accounting and rewind: PASS");
    return 0;
}
