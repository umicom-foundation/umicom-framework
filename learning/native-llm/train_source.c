/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/train_source.c
 *
 * PURPOSE:
 *   Thin educational consumer: train a byte-level decoder on a reviewed,
 *   immutable multi-file source-code dataset with distinct held-out files.
 *   Everything reusable lives in the Framework's training library.
 *
 * SAFETY:
 *   This executable reads local reviewed files. By default it only reports
 *   scalar losses; it writes a new checkpoint ONLY when an explicit output
 *   path is provided. It never uploads or executes training source code.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/dataset_manifest.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/checkpoint.h"
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define UMI_SOURCE_CONTEXT 8U
#define UMI_SOURCE_MAX_STEPS 512U
#define UMI_SOURCE_EVALUATIONS 5U

static int ParseSteps(const char *text, size_t *outSteps)
{
    char *end = NULL;
    unsigned long number;
    if (text == NULL || text[0] == '\0' || outSteps == NULL || text[0] == '-')
        return 0;
    errno = 0;
    number = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || number == 0UL ||
        number > (unsigned long)UMI_SOURCE_MAX_STEPS)
        return 0;
    *outSteps = (size_t)number;
    return 1;
}

static UmiStatus EvaluateGroup(UmiAiTrainingDecoder *model,
                               const UmiAiTrainingDataset *dataset,
                               uint64_t seed, float *outMeanLoss)
{
    double total = 0.0;
    for (size_t batch = 0U; batch < UMI_SOURCE_EVALUATIONS; ++batch) {
        size_t inputs[UMI_SOURCE_CONTEXT], targets[UMI_SOURCE_CONTEXT];
        float loss;
        UmiStatus status = UmiAiTrainingDatasetBatch(dataset,
            UMI_AI_TRAINING_PARTITION_EVALUATE, UMI_SOURCE_CONTEXT, 1U,
            seed, (uint64_t)batch, inputs, targets, UMI_SOURCE_CONTEXT);
        if (status != UMI_STATUS_OK) return status;
        status = UmiAiTrainingDecoderEvaluate(model, inputs, targets,
                                              UMI_SOURCE_CONTEXT, &loss);
        if (status != UMI_STATUS_OK || !isfinite(loss)) return UMI_STATUS_INVALID_STATE;
        total += (double)loss;
    }
    *outMeanLoss = (float)(total / (double)UMI_SOURCE_EVALUATIONS);
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingDatasetSummary summary;
    UmiAiTrainingDecoderConfig configuration = {
        UMI_AI_BYTE_VOCABULARY, UMI_SOURCE_CONTEXT, 8U, 16U, 2U,
        UINT64_C(20261021)
    };
    uint64_t stepCount = 0U;
    float baseline, finalLoss, trainLoss = 0.0F;
    size_t iterations = 64U;
    UmiStatus status;
    if (argc < 2 || argc > 4 || (argc >= 3 && !ParseSteps(argv[2], &iterations))) {
        fprintf(stderr, "usage: umicom-native-llm-source-train <trusted-manifest> [steps 1..512] [new-checkpoint-path]\n");
        return 2;
    }
    status = UmiAiTrainingDatasetLoadManifest(argv[1], &dataset);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "source manifest rejected (status=%d)\n", (int)status);
        return 1;
    }
    status = UmiAiTrainingDatasetInspect(dataset, &summary);
    if (status != UMI_STATUS_OK) goto failed;
    printf("Training files: %zu (%zu bytes); held-out files: %zu (%zu bytes)\n",
           summary.trainingDocuments, summary.trainingBytes,
           summary.evaluationDocuments, summary.evaluationBytes);
    for (size_t index = 0U; index < summary.documentCount; ++index) {
        UmiAiTrainingDocumentInfo document;
        status = UmiAiTrainingDatasetDocument(dataset, index, &document);
        if (status != UMI_STATUS_OK) goto failed;
        printf("  %-5s %s  bytes=%zu fnv64=%016" PRIx64 "\n",
               document.partition == UMI_AI_TRAINING_PARTITION_TRAIN ?
                   "train" : "eval", document.relativePath,
               document.byteCount, document.fingerprint);
    }
    status = UmiAiTrainingDecoderCreate(&configuration, &model);
    if (status != UMI_STATUS_OK) goto failed;
    status = EvaluateGroup(model, dataset, UINT64_C(5544), &baseline);
    if (status != UMI_STATUS_OK) goto failed;
    printf("Baseline held-out cross-entropy: %.6f\n", (double)baseline);
    for (size_t step = 0U; step < iterations; ++step) {
        size_t inputs[UMI_SOURCE_CONTEXT], targets[UMI_SOURCE_CONTEXT];
        status = UmiAiTrainingDatasetBatch(dataset,
            UMI_AI_TRAINING_PARTITION_TRAIN, UMI_SOURCE_CONTEXT, 1U,
            UINT64_C(4477), (uint64_t)step, inputs, targets,
            UMI_SOURCE_CONTEXT);
        if (status != UMI_STATUS_OK) goto failed;
        status = UmiAiTrainingDecoderTrainStep(model, inputs, targets,
            UMI_SOURCE_CONTEXT, 0.025F, &trainLoss);
        if (status != UMI_STATUS_OK || !isfinite(trainLoss)) goto failed;
        if ((step + 1U) % 16U == 0U || step + 1U == iterations) {
            printf("step %zu/%zu: last sampled training loss %.6f\n",
                   step + 1U, iterations, (double)trainLoss);
        }
    }
    status = EvaluateGroup(model, dataset, UINT64_C(5544), &finalLoss);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDecoderInspect(model, &configuration, &stepCount);
    if (status != UMI_STATUS_OK) goto failed;
    printf("Held-out cross-entropy before %.6f, after %.6f; completed steps=%" PRIu64 "\n",
           (double)baseline, (double)finalLoss, stepCount);
    puts("These are tiny byte-model experiments, NOT coding expertise."
         " A lower training loss does not guarantee better held-out results.");
    if (argc == 4) {
        status = UmiAiTrainingDecoderCheckpointSaveNewFile(model, argv[3]);
        if (status != UMI_STATUS_OK) {
            fprintf(stderr, "checkpoint publication failed (status=%d)\n", (int)status);
            goto failed;
        }
        printf("New checkpoint saved: %s\n", argv[3]);
    }
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    return 0;
failed:
    fprintf(stderr, "native source training failed (status=%d)\n", (int)status);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    return 1;
}
