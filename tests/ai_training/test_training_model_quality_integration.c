/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_model_quality_integration.c
 * PURPOSE: Exercise the real decoder and immutable source-split evaluator;
 *          prove an identical checkpoint cannot pass as a new candidate.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_quality_gate.h"

#include <stdio.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH must be defined by CMake
#endif
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingExperimentConfig plan, actual;
    UmiAiTrainingModelQualityPolicy policy;
    UmiAiTrainingModelQualityReport result;
    uint64_t beforeSteps = UINT64_C(987);
    uint64_t afterSteps = UINT64_C(987);
    CHECK(UmiAiTrainingExperimentDefaults(&plan) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingModelQualityDefaults(&policy) == UMI_STATUS_OK);
    plan.steps = 4U;
    plan.evalInterval = 2U;
    plan.evalBatches = 1U;
    plan.batchSequences = 1U;
    policy.minimumEvaluatedTokens = (size_t)plan.decoder.contextLength;
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
                                           &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&plan.decoder, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderInspect(model, &actual, &beforeSteps) == UMI_STATUS_OK);
    CHECK(beforeSteps == UINT64_C(0));
    CHECK(UmiAiTrainingModelQualityEvaluate(
        model, model, dataset, &plan, &policy, &result) == UMI_STATUS_OK);
    CHECK(result.accepted == 0);
    CHECK(result.baseline.evaluatedTokens == result.candidate.evaluatedTokens);
    CHECK(result.baseline.planFingerprint == result.candidate.planFingerprint);
    CHECK(result.baseline.datasetFingerprint == result.candidate.datasetFingerprint);
    CHECK(result.baseline.modelFingerprint == result.candidate.modelFingerprint);
    CHECK(UmiAiTrainingDecoderInspect(model, &actual, &afterSteps) == UMI_STATUS_OK);
    CHECK(afterSteps == beforeSteps);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("native model quality: real decoder held-out audit and no-change identity PASS");
    return 0;
}
