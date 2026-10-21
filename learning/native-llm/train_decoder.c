/*-----------------------------------------------------------------------------
 * Umicom Framework | learning/native-llm/train_decoder.c
 *
 * PURPOSE:
 *   Demonstrate a complete native decoder: token + learned position lookup,
 *   a two-head pre-LN Transformer Block, logits and cross-entropy optimisation.
 *   Train on a deterministic sequence partition, inspect both losses, then
 *   restore an exact model checkpoint using Framework-owned binary semantics.
 *
 * LIMITATIONS:
 *   Toy token IDs; not a general tokenizer or a capable coding model.
 *   The twelve example pairs and three held-out combinations are far too
 *   small to claim language generalisation. The seeded SGD experiment uses
 *   no dropout, augmentation, shuffling per epoch, or optimiser momentum.
 *
 * USAGE:
 *   umicom-native-llm-decoder-demo [new_checkpoint_file]
 *   With no filename, demonstrate an in-memory checkpoint roundtrip.
 *   With a filename, save NEW only, refusing an existing checkpoint.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/dataset_split.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define REQUIRE(call) do { \
    UmiStatus result = (call); \
    if (result != UMI_STATUS_OK) { \
        fprintf(stderr, "Umicom decoder lab, line %d: %s (status %d)\n", \
                __LINE__, #call, (int)result); \
        UmiAiTrainingDecoderDestroy(model); \
        UmiAiTrainingDecoderDestroy(reloaded); \
        return 1; \
    } \
} while (0)

int main(int argc, char **argv)
{
    UmiAiTrainingDecoderConfig config = {4U, 2U, 4U, 8U, 2U, UINT64_C(20261021)};
    UmiAiTrainingDecoder *model = NULL, *reloaded = NULL;
    size_t order[12], trainInputs[18], trainTargets[18], evalInputs[6], evalTargets[6];
    float firstTrain = 0.0F, lastTrain = 0.0F;
    float firstEval = 0.0F, lastEval = 0.0F;
    size_t checkpointSize = 0U, written = 0U;
    uint8_t *snapshot = NULL;
    const char *const names[4] = {"Umicom", "C23", "Calypso", "Framework"};
    if (argc > 2) { fputs("usage: decoder-demo [new_checkpoint_file]\n", stderr); return 2; }
    REQUIRE(UmiAiTrainingDecoderCreate(&config, &model));
    REQUIRE(UmiAiTrainingSplitSequences(12U, 3U, UINT64_C(321), order, 12U));
    for (size_t i = 0U; i < 12U; ++i) {
        size_t value = order[i];
        size_t left = value / 3U, right = value % 3U;
        if (right >= left) right++;
        size_t *source = i < 9U ? trainInputs+2U*i : evalInputs+2U*(i-9U);
        size_t *target = i < 9U ? trainTargets+2U*i : evalTargets+2U*(i-9U);
        source[0] = left; source[1] = right;
        target[0] = right; target[1] = left;
    }
    puts("Umicom native C23 decoder — learned positions, multi-head block, logits");
    puts("Scratch random weights; 9 distinct training pairs; 3 held-out pairs.");
    REQUIRE(UmiAiTrainingDecoderEvaluate(model, trainInputs, trainTargets, 18U, &firstTrain));
    REQUIRE(UmiAiTrainingDecoderEvaluate(model, evalInputs, evalTargets, 6U, &firstEval));
    for (size_t epoch = 0U; epoch < 420U; ++epoch) {
        float stepLoss = 0.0F;
        REQUIRE(UmiAiTrainingDecoderTrainStep(model, trainInputs, trainTargets,
                                              18U, 0.08F, &stepLoss));
        if (epoch == 0U || epoch == 99U || epoch == 299U || epoch == 419U)
            printf("step %3zu training-batch loss %.6f\n", epoch+1U, (double)stepLoss);
    }
    REQUIRE(UmiAiTrainingDecoderEvaluate(model, trainInputs, trainTargets, 18U, &lastTrain));
    REQUIRE(UmiAiTrainingDecoderEvaluate(model, evalInputs, evalTargets, 6U, &lastEval));
    printf("training cross-entropy %.6f -> %.6f\n",(double)firstTrain,(double)lastTrain);
    printf("held-out cross-entropy %.6f -> %.6f\n",(double)firstEval,(double)lastEval);
    if (!isfinite(lastTrain) || !isfinite(lastEval) || lastTrain >= firstTrain) {
        UmiAiTrainingDecoderDestroy(model);
        return 1;
    }
    size_t prefix[2] = {0U,2U}, predicted = 0U;
    REQUIRE(UmiAiTrainingDecoderPredictNext(model, prefix, 2U, &predicted));
    printf("greedy next token after '%s %s': '%s'\n",
           names[prefix[0]], names[prefix[1]], names[predicted]);

    if (argc == 2) {
        REQUIRE(UmiAiTrainingDecoderCheckpointSaveNewFile(model, argv[1]));
        REQUIRE(UmiAiTrainingDecoderCheckpointLoadFile(argv[1], &reloaded));
        printf("published a new checksum-checked checkpoint: %s\n", argv[1]);
    } else {
        REQUIRE(UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize));
        snapshot = malloc(checkpointSize);
        if (snapshot == NULL) { UmiAiTrainingDecoderDestroy(model); return 1; }
        UmiStatus encodeResult = UmiAiTrainingDecoderCheckpointEncode(model,
                                            snapshot, checkpointSize, &written);
        if (encodeResult != UMI_STATUS_OK) {
            free(snapshot); UmiAiTrainingDecoderDestroy(model); return 1;
        }
        UmiStatus decodeResult = UmiAiTrainingDecoderCheckpointDecode(snapshot,
                                             written, &reloaded);
        free(snapshot);
        if (decodeResult != UMI_STATUS_OK) {
            UmiAiTrainingDecoderDestroy(model); return 1;
        }
        printf("restored %zu checkpoint bytes in memory\n", written);
    }
    size_t afterRestore = 0U;
    REQUIRE(UmiAiTrainingDecoderPredictNext(reloaded, prefix, 2U, &afterRestore));
    if (afterRestore != predicted) {
        fputs("checkpoint roundtrip changed inference\n", stderr);
        UmiAiTrainingDecoderDestroy(model);
        UmiAiTrainingDecoderDestroy(reloaded);
        return 1;
    }
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDecoderDestroy(reloaded);
    puts("Checkpoint reloaded; prediction identical. No expert-model claim.");
    return 0;
}
