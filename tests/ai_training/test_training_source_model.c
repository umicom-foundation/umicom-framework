/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_source_model.c
 * Purpose: Use real C23 source bytes with the canonical decoder at 260 tokens,
 *          and confirm held-out evaluation cannot update model parameters.
 * Author: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/dataset_manifest.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; \
} } while (0)
#ifndef UMI_AI_TEST_MANIFEST_PATH
#error CMake must supply the approved source fixture manifest.
#endif
int main(void)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL, *restored = NULL;
    UmiAiTrainingDecoderConfig config = {UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, 777U};
    UmiAiTrainingDecoderConfig actual = {0};
    uint64_t steps = 0U;
    float initialLoss = 0.0F, currentLoss = 0.0F, heldOutBefore, heldOutAfter;
    size_t trainInputs[8], trainTargets[8], evalInputs[8], evalTargets[8];
    size_t savedSize = 0U, written = 0U;
    uint8_t *buffer;
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
                                          &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetBatch(dataset, UMI_AI_TRAINING_PARTITION_TRAIN,
        8U, 1U, 555U, 1U, trainInputs, trainTargets, 8U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetBatch(dataset, UMI_AI_TRAINING_PARTITION_EVALUATE,
        8U, 1U, 444U, 1U, evalInputs, evalTargets, 8U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderEvaluate(model, trainInputs, trainTargets,
                                       8U, &initialLoss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderEvaluate(model, evalInputs, evalTargets,
                                       8U, &heldOutBefore) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 25U; ++i) {
        CHECK(UmiAiTrainingDecoderTrainStep(model, trainInputs, trainTargets,
                                            8U, 0.025F, &currentLoss) == UMI_STATUS_OK);
    }
    CHECK(UmiAiTrainingDecoderEvaluate(model, trainInputs, trainTargets,
                                       8U, &currentLoss) == UMI_STATUS_OK);
    CHECK(isfinite(currentLoss) && currentLoss < initialLoss);
    CHECK(UmiAiTrainingDecoderEvaluate(model, evalInputs, evalTargets,
                                       8U, &heldOutAfter) == UMI_STATUS_OK);
    CHECK(isfinite(heldOutBefore) && isfinite(heldOutAfter));
    CHECK(UmiAiTrainingDecoderInspect(model, &actual, &steps) == UMI_STATUS_OK);
    CHECK(steps == 25U && actual.vocabularySize == UMI_AI_BYTE_VOCABULARY);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &savedSize) == UMI_STATUS_OK);
    CHECK(savedSize <= UMI_AI_DECODER_MAX_CHECKPOINT_BYTES);
    buffer = malloc(savedSize);
    CHECK(buffer != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, buffer,
                                               savedSize, &written) == UMI_STATUS_OK);
    CHECK(written == savedSize);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(buffer, written,
                                               &restored) == UMI_STATUS_OK);
    free(buffer);
    CHECK(UmiAiTrainingDecoderInspect(restored, &actual, &steps) == UMI_STATUS_OK);
    CHECK(steps == 25U && actual.vocabularySize == UMI_AI_BYTE_VOCABULARY);
    CHECK(UmiAiTrainingDecoderEvaluate(restored, evalInputs, evalTargets,
                                       8U, &initialLoss) == UMI_STATUS_OK);
    CHECK(initialLoss == heldOutAfter);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("PASS: 260-token source decoder training, evaluation and checkpoint");
    return 0;
}
