/*-----------------------------------------------------------------------------
 * Umicom Framework — Native language-model learning laboratory
 * File: learning/native-llm/train_adamw.c
 * PURPOSE:
 *   Train the canonical native decoder using stateful AdamW and a learning-
 *   rate schedule. Source selection and all neural maths belong to Framework.
 *   This sample does not write or pretend to restore AdamW moment state.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * LICENCE: MIT
 */
#include "umicom/ai/training/dataset_manifest.h"
#include "umicom/ai/training/optimizer.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingAdamW *optimizer = NULL;
    UmiAiTrainingAdamWConfig adamOptions;
    UmiAiTrainingDecoderConfig modelConfig = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261021)};
    UmiAiTrainingLearningRateSchedule schedule = {
        UMI_AI_TRAINING_SCHEDULE_COSINE, 0.025F, 0.0025F, 2U, 12U};
    size_t inputs[8], targets[8];
    UmiStatus status;
    float baseline = 0.0F, loss = 0.0F, finalLoss = 0.0F;
    uint64_t steps = 0U;
    size_t stateBytes = 0U;
    if (argc != 2) {
        fprintf(stderr, "usage: umicom-native-llm-adamw <reviewed-source-manifest>\n");
        return 2;
    }
    status = UmiAiTrainingDatasetLoadManifest(argv[1], &dataset);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDecoderCreate(&modelConfig, &model);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingAdamWDefaults(&adamOptions);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDecoderAdamWCreate(model, &adamOptions, &optimizer);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDatasetBatch(dataset,
        UMI_AI_TRAINING_PARTITION_EVALUATE, 8U, 1U, UINT64_C(5544), 0U,
        inputs, targets, 8U);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDecoderEvaluate(model, inputs, targets, 8U, &baseline);
    if (status != UMI_STATUS_OK) goto failed;
    puts("Umicom native AdamW — sampled C23 source training");
    for (uint64_t step = UINT64_C(1); step <= schedule.totalSteps; ++step) {
        float rate = 0.0F;
        status = UmiAiTrainingLearningRateForStep(&schedule, step, &rate);
        if (status != UMI_STATUS_OK) goto failed;
        status = UmiAiTrainingDatasetBatch(dataset,
            UMI_AI_TRAINING_PARTITION_TRAIN, 8U, 1U,
            UINT64_C(4477), step - UINT64_C(1), inputs, targets, 8U);
        if (status != UMI_STATUS_OK) goto failed;
        status = UmiAiTrainingDecoderTrainAdamWStep(model, optimizer,
            inputs, targets, 8U, rate, &loss);
        if (status != UMI_STATUS_OK) goto failed;
        if (step == 1U || step == schedule.totalSteps || step % 4U == 0U)
            printf("step %2" PRIu64 ": rate %.6f training loss %.6f\n",
                   step, (double)rate, (double)loss);
    }
    status = UmiAiTrainingDatasetBatch(dataset,
        UMI_AI_TRAINING_PARTITION_EVALUATE, 8U, 1U,
        UINT64_C(5544), 0U, inputs, targets, 8U);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDecoderEvaluate(model, inputs, targets, 8U, &finalLoss);
    if (status != UMI_STATUS_OK || !isfinite(finalLoss)) goto failed;
    status = UmiAiTrainingAdamWInspect(optimizer, &steps, &stateBytes);
    if (status != UMI_STATUS_OK) goto failed;
    printf("held-out CE: %.6f -> %.6f; steps=%" PRIu64 "; AdamW state=%zu bytes\n",
           (double)baseline, (double)finalLoss, steps, stateBytes);
    puts("This is a small training experiment, not programming expertise.");
    puts("A model-only checkpoint is not sufficient to resume AdamW exactly.");
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    return 0;
failed:
    fprintf(stderr, "native AdamW laboratory rejected (status=%d)\n", (int)status);
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    return 1;
}
