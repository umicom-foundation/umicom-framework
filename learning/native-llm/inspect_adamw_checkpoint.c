/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Laboratory
 * File: learning/native-llm/inspect_adamw_checkpoint.c
 * PURPOSE:
 *   Demonstrate a deliberate NEW combined AdamW checkpoint publication and
 *   exact in-process continuation on reviewed source-code samples. The
 *   canonical Framework owns every computation and checkpoint file operation.
 *   This program does not execute source text or contact network services.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 */
#include "umicom/ai/training/optimizer_checkpoint.h"
#include "umicom/ai/training/dataset_manifest.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <inttypes.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261021)};
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL, *restored = NULL;
    UmiAiTrainingAdamW *optimizer = NULL, *recovered = NULL;
    UmiAiTrainingAdamWConfig options;
    size_t input[8], targets[8];
    UmiStatus status;
    float loss = 0.0F;
    uint64_t steps = 0U;
    size_t stateBytes = 0U;
    if (argc != 3) {
        fprintf(stderr, "usage: umicom-native-llm-adamw-checkpoint "
                        "<reviewed-source-manifest> <NEW-checkpoint-path>\n");
        return 2;
    }
    status = UmiAiTrainingDatasetLoadManifest(argv[1], &dataset);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDecoderCreate(&config, &model);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingAdamWDefaults(&options);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDecoderAdamWCreate(model, &options, &optimizer);
    if (status != UMI_STATUS_OK) goto failed;
    for (uint64_t step = UINT64_C(0); step < UINT64_C(4); ++step) {
        status = UmiAiTrainingDatasetBatch(dataset,
            UMI_AI_TRAINING_PARTITION_TRAIN, 8U, 1U, UINT64_C(4477),
            step, input, targets, 8U);
        if (status != UMI_STATUS_OK) goto failed;
        status = UmiAiTrainingDecoderTrainAdamWStep(model, optimizer,
                                                    input, targets, 8U, 0.015F, &loss);
        if (status != UMI_STATUS_OK) goto failed;
    }
    /* No CLI file writer or shell command. This refuses any existing target. */
    status = UmiAiTrainingAdamWCheckpointSaveNewFile(model, optimizer, argv[2]);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingAdamWCheckpointLoadFile(argv[2], &restored, &recovered);
    if (status != UMI_STATUS_OK) goto failed;
    for (uint64_t step = UINT64_C(4); step < UINT64_C(8); ++step) {
        status = UmiAiTrainingDatasetBatch(dataset,
            UMI_AI_TRAINING_PARTITION_TRAIN, 8U, 1U, UINT64_C(4477),
            step, input, targets, 8U);
        if (status != UMI_STATUS_OK) goto failed;
        status = UmiAiTrainingDecoderTrainAdamWStep(restored, recovered,
                                                    input, targets, 8U, 0.015F, &loss);
        if (status != UMI_STATUS_OK) goto failed;
    }
    status = UmiAiTrainingAdamWInspect(recovered, &steps, &stateBytes);
    if (status != UMI_STATUS_OK) goto failed;
    printf("Combined checkpoint: %s\n", argv[2]);
    printf("Restored AdamW optimiser steps: %" PRIu64 ", "
           "state allocation: %zu bytes\n", steps, stateBytes);
    puts("Reproduction requires the identical reviewed dataset, seeds and schedule.");
    puts("This is experimental training, not evidence of task proficiency.");
    UmiAiTrainingAdamWDestroy(recovered);
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    return 0;
failed:
    fprintf(stderr, "AdamW checkpoint demo failed (status=%d)\n", (int)status);
    UmiAiTrainingAdamWDestroy(recovered);
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    return 1;
}
