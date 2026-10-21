/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_model_audit.c
 *
 * PURPOSE: Confirm reproducible held-out evidence and cross-API agreement.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_audit.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH must be supplied by the build system
#endif
#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingExperimentConfig plan;
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingModelAudit first = {0}, again = {0};
    UmiAiTrainingDecoderConfig modelConfig;
    uint64_t steps = UINT64_MAX;
    double mean = -1.0, perplexity = -1.0;
    int matches = -1;
    size_t length = 0U, written = 0U;
    uint8_t *before = NULL, *after = NULL;

    CHECK(UmiAiTrainingExperimentDefaults(&plan) == UMI_STATUS_OK);
    plan.steps = 4U;
    plan.evalInterval = 2U;
    plan.evalBatches = 2U;
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
                                           &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&plan.decoder, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &length) == UMI_STATUS_OK);
    before = malloc(length);
    after = malloc(length);
    CHECK(before != NULL && after != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, before,
                                               length, &written) == UMI_STATUS_OK);
    CHECK(written == length);
    CHECK(UmiAiTrainingModelAuditEvaluate(model, dataset, &plan,
                                           &first) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingModelAuditEvaluate(model, dataset, &plan,
                                           &again) == UMI_STATUS_OK);
    CHECK(first.modelFingerprint == again.modelFingerprint);
    CHECK(first.datasetFingerprint == again.datasetFingerprint);
    CHECK(first.planFingerprint == again.planFingerprint);
    CHECK(first.combinedFingerprint == again.combinedFingerprint);
    CHECK(first.evaluatedTokens == 16U);
    CHECK(first.topOneCorrect <= first.topFiveCorrect);
    CHECK(first.topFiveCorrect <= first.evaluatedTokens);
    CHECK(first.heldOutCrossEntropy == again.heldOutCrossEntropy);
    CHECK(first.heldOutPerplexity == again.heldOutPerplexity);
    CHECK(UmiAiTrainingExperimentEvaluate(model, dataset, &plan,
                                  &mean, &perplexity) == UMI_STATUS_OK);
    CHECK(mean == first.heldOutCrossEntropy);
    CHECK(perplexity == first.heldOutPerplexity);
    CHECK(isfinite(mean) && isfinite(perplexity) && perplexity >= 1.0);
    CHECK(UmiAiTrainingDecoderInspect(model, &modelConfig, &steps) == UMI_STATUS_OK);
    CHECK(steps == 0U && modelConfig.vocabularySize == plan.decoder.vocabularySize);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after,
                                               length, &written) == UMI_STATUS_OK);
    CHECK(memcmp(before, after, length) == 0);
    CHECK(UmiAiTrainingModelAuditVerify(&first, model, dataset, &plan,
                                        &matches) == UMI_STATUS_OK);
    CHECK(matches == 1);
    free(after);
    free(before);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("held-out audit, deterministic identities and immutable model: PASS");
    return 0;
}
