/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_inference_checkpoint.c
 * PURPOSE: Qualify profiling on trained and independently restored weights;
 *          the observer must preserve the exact checkpoint bytes.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/inference_profile.h"
#include "umicom/ai/training/checkpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {17U, 6U, 8U, 16U, 2U, UINT64_C(101)};
    const size_t inputs[] = {4U, 5U, 6U, 7U, 8U, 9U};
    const size_t targets[] = {5U, 6U, 7U, 8U, 9U, 10U};
    const size_t stream[] = {4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingDecoder *restored = NULL;
    UmiAiTrainingInferenceProfile measured = {0};
    UmiAiTrainingInferenceProfile reloaded = {0};
    uint8_t *before = NULL;
    uint8_t *after = NULL;
    size_t encodedBytes = 0U;
    size_t size = 0U;
    size_t writtenBefore = 0U, writtenAfter = 0U;
    float loss = 0.0F;

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainStep(model, inputs, targets, 6U,
                                        0.01F, &loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &size) == UMI_STATUS_OK);
    CHECK(size > 0U);
    before = malloc(size);
    after = malloc(size);
    CHECK(before != NULL && after != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, before, size,
                                                &writtenBefore) == UMI_STATUS_OK);
    CHECK(writtenBefore == size);
    CHECK(UmiAiTrainingInferenceProfileStream(model, stream, 9U, 3U,
                                                &measured) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after, size,
                                                &writtenAfter) == UMI_STATUS_OK);
    CHECK(writtenAfter == size);
    CHECK(memcmp(before, after, size) == 0);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(before, size,
                                                &restored) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingInferenceProfileStream(restored, stream, 9U, 3U,
                                                &reloaded) == UMI_STATUS_OK);
    CHECK(measured.referenceQueries == reloaded.referenceQueries);
    CHECK(measured.cacheComputedRows == reloaded.cacheComputedRows);
    CHECK(measured.cacheWindowRebuilds == reloaded.cacheWindowRebuilds);
    CHECK(measured.footprint.logicalParameterBytes ==
          reloaded.footprint.logicalParameterBytes);
    CHECK(measured.maximumLogitDeviation == reloaded.maximumLogitDeviation);
    /* Do not assert equal CPU times: those are measurements, not invariants. */
    CHECK(UmiAiTrainingDecoderCheckpointSize(restored,
                                              &encodedBytes) == UMI_STATUS_OK);
    CHECK(encodedBytes == size);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(model);
    free(after);
    free(before);
    puts("profiling leaves trained checkpoints unchanged after restoration: PASS");
    return 0;
}
