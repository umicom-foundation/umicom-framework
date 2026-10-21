/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM learning laboratory
 * File: learning/native-llm/audit_model.c
 *
 * PURPOSE:
 *   Inspect an approved decoder checkpoint against a reviewed experiment plan
 *   and immutable held-out source manifest. Print a reproducible identity and
 *   real model evaluation; never publish or execute generated code.
 *
 * ARCHITECTURE:
 *   All hashing, data access, evaluation and model operations are delegated
 *   to Framework services. No separate application-local scoring engine.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/checkpoint.h"
#include "umicom/ai/training/experiment_plan.h"
#include "umicom/ai/training/model_audit.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define UMI_AUDIT_CLI_MANIFEST_CAPACITY 1024U

static int ParseFingerprint(const char *text, uint64_t *outValue)
{
    uint64_t value = 0U;
    if (text == NULL || outValue == NULL || strlen(text) != 16U) return 0;
    for (size_t index = 0U; index < 16U; ++index) {
        unsigned nibble;
        if (text[index] >= '0' && text[index] <= '9')
            nibble = (unsigned)(text[index] - '0');
        else if (text[index] >= 'a' && text[index] <= 'f')
            nibble = (unsigned)(text[index] - 'a') + 10U;
        else if (text[index] >= 'A' && text[index] <= 'F')
            nibble = (unsigned)(text[index] - 'A') + 10U;
        else return 0;
        value = (value << 4U) | (uint64_t)nibble;
    }
    *outValue = value;
    return 1;
}

int main(int argc, char **argv)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingExperimentConfig plan;
    UmiAiTrainingModelAudit report;
    char manifest[UMI_AUDIT_CLI_MANIFEST_CAPACITY];
    uint64_t expected = 0U;
    UmiStatus status;

    if ((argc != 3 && argc != 4) ||
        (argc == 4 && !ParseFingerprint(argv[3], &expected))) {
        fputs("usage: umicom-native-llm-audit <approved-plan> "
              "<trusted-checkpoint> [expected-16-digit-hex-fingerprint]\n",
              stderr);
        return 2;
    }
    status = UmiAiTrainingExperimentPlanLoadFile(argv[1], &plan,
                                                 manifest, sizeof(manifest));
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDatasetLoadManifest(manifest, &dataset);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDecoderCheckpointLoadFile(argv[2], &model);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingModelAuditEvaluate(model, dataset, &plan, &report);
    if (status != UMI_STATUS_OK) goto failed;

    printf("Umicom native decoder held-out evidence\n"
           "Model:   %016" PRIx64 "\n"
           "Dataset: %016" PRIx64 "\n"
           "Plan:    %016" PRIx64 "\n"
           "Combined fingerprint: %016" PRIx64 "\n"
           "Completed optimiser steps: %" PRIu64 "\n"
           "Training documents: %zu; held-out documents: %zu\n"
           "Evaluated target tokens: %zu\n"
           "Held-out cross-entropy: %.7f\n"
           "Held-out perplexity: %.7f\n"
           "Top-1 next-token accuracy: %zu/%zu (%.2f%%)\n"
           "Top-5 next-token accuracy: %zu/%zu (%.2f%%)\n",
           report.modelFingerprint, report.datasetFingerprint,
           report.planFingerprint, report.combinedFingerprint,
           report.completedSteps, report.dataset.trainingDocuments,
           report.dataset.evaluationDocuments, report.evaluatedTokens,
           report.heldOutCrossEntropy, report.heldOutPerplexity,
           report.topOneCorrect, report.evaluatedTokens,
           100.0 * (double)report.topOneCorrect / (double)report.evaluatedTokens,
           report.topFiveCorrect, report.evaluatedTokens,
           100.0 * (double)report.topFiveCorrect / (double)report.evaluatedTokens);
    puts("The sample is fixed-seed held-out evaluation, not an exhaustive corpus test.");
    puts("Fingerprints use FNV-1a and DO NOT authenticate checkpoint or dataset files.");
    if (argc == 4 && expected != report.combinedFingerprint) {
        fprintf(stderr, "reviewed identity does not match (expected %016" PRIx64 ")\n",
                expected);
        UmiAiTrainingDecoderDestroy(model);
        UmiAiTrainingDatasetDestroy(dataset);
        return 1;
    }
    if (argc == 4) puts("Reviewed identity matches.");
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    return 0;

failed:
    fprintf(stderr, "native model audit failed (status=%d)\n", (int)status);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    return 1;
}
