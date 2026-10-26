/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_experiment_rejections.c
 * PURPOSE: Validate run plans and preflight rejections BEFORE SGD side effects.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "supervised_fixture.h"
#include "umicom/ai/training/supervised_experiment.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "experiment refusal:%d %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    UmiTestSupervisedFixture fixture;
    UmiAiTrainingDecoder *decoder = NULL;
    UmiAiTrainingDecoderProjectionAdapter *adapter = NULL;
    UmiAiTrainingSupervisedDataset *dataset = NULL;
    UmiAiTrainingDecoderProjectionAdapterInfo before = {0}, after = {0};
    UmiAiTrainingSupervisedExperimentReport report, saved;
    UmiAiTrainingSupervisedExperimentPlan plan = {
        2U, 1U, UINT64_C(2), 0.01F, 0.0
    };
    const UmiAiTrainingDecoderConfig config = {
        260U, 8U, 8U, 16U, 2U, UINT64_C(9001)
    };
    CHECK(UmiTestSupervisedFixturePrepare(&fixture) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedDatasetCreate(fixture.records, fixture.lengths,
          fixture.evidence, 4U, 8U, &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&config, &decoder) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderProjectionAdapterCreate(decoder,
          UMI_AI_DECODER_ADAPT_VALUE, 2U, UINT64_C(7), &adapter) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderProjectionAdapterInspect(adapter, &before) == UMI_STATUS_OK);
    memset(&report, 0x5a, sizeof(report));
    saved = report;
    plan.learningRate = NAN;
    CHECK(UmiAiTrainingSupervisedExperimentRun(adapter, dataset, &plan,
          &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &saved, sizeof(report)) == 0);
    plan.learningRate = 0.01F;
    plan.evaluateEveryEpochs = 3U;
    CHECK(UmiAiTrainingSupervisedExperimentRun(adapter, dataset, &plan,
          &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &saved, sizeof(report)) == 0);
    plan.evaluateEveryEpochs = 1U;
    plan.stopAtEvaluationLoss = -1.0;
    CHECK(UmiAiTrainingSupervisedExperimentRun(adapter, dataset, &plan,
          &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &saved, sizeof(report)) == 0);
    CHECK(UmiAiTrainingDecoderProjectionAdapterInspect(adapter, &after) == UMI_STATUS_OK);
    CHECK(after.completedAdapterSteps == before.completedAdapterSteps);
    UmiAiTrainingSupervisedEvaluation eval = {0};
    CHECK(UmiAiTrainingSupervisedExperimentEvaluate(adapter, dataset,
          &eval) == UMI_STATUS_OK);
    CHECK(eval.exampleCount == 2U && eval.supervisedTokens >= 4U);
    UmiAiTrainingDecoderProjectionAdapterDestroy(adapter);
    UmiAiTrainingDecoderDestroy(decoder);
    UmiAiTrainingSupervisedDatasetDestroy(dataset);
    puts("Invalid run policies reject without publishing output or mutating adapters");
    return 0;
}
