/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_experiment.c
 * PURPOSE:
 *   End-to-end native test: reviewed synthetic sources -> response-only byte
 *   windows -> frozen decoder projection SGD -> held-out evaluation -> receipt.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "supervised_fixture.h"
#include "umicom/ai/training/supervised_experiment.h"
#include "umicom/ai/training/supervised_receipt.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "experiment:%d %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        260U, 8U, 8U, 16U, 2U, UINT64_C(70211)
    };
    UmiTestSupervisedFixture fixture;
    UmiAiTrainingDecoder *decoder = NULL;
    UmiAiTrainingDecoderProjectionAdapter *adapter = NULL;
    UmiAiTrainingSupervisedDataset *dataset = NULL;
    UmiAiTrainingSupervisedExperimentReport report = {0};
    UmiAiTrainingSupervisedReceiptInfo parsed = {0};
    UmiAiTrainingDecoderProjectionAdapterInfo current = {0};
    const UmiAiTrainingSupervisedExperimentPlan plan = {
        2U, 1U, UINT64_C(202611), 0.025F, 0.0
    };
    uint8_t *checkpointBefore = NULL, *checkpointAfter = NULL;
    size_t checkpointSize = 0U, beforeLength = 0U, afterLength = 0U;
    uint8_t receipt[UMI_AI_SUPERVISED_RECEIPT_BYTES] = {0};
    size_t written = 0U;

    CHECK(UmiTestSupervisedFixturePrepare(&fixture) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedDatasetCreate(
          fixture.records, fixture.lengths, fixture.evidence, 4U, 8U,
          &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&config, &decoder) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(decoder, &checkpointSize) == UMI_STATUS_OK);
    checkpointBefore = malloc(checkpointSize);
    checkpointAfter = malloc(checkpointSize);
    CHECK(checkpointBefore != NULL && checkpointAfter != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(decoder, checkpointBefore,
          checkpointSize, &beforeLength) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderProjectionAdapterCreate(decoder,
          UMI_AI_DECODER_ADAPT_QUERY | UMI_AI_DECODER_ADAPT_VALUE,
          2U, UINT64_C(28), &adapter) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedExperimentRun(adapter, dataset,
          &plan, &report) == UMI_STATUS_OK);
    CHECK(report.completedEpochs == 2U && report.completedTrainingSteps == 4U);
    CHECK(report.initialAdapterSteps == 0U && report.finalAdapterSteps == 4U);
    CHECK(report.before.supervisedTokens == report.dataset.evaluationTargets);
    CHECK(report.after.supervisedTokens == report.dataset.evaluationTargets);
    CHECK(isfinite(report.before.meanCrossEntropy) &&
          isfinite(report.after.meanCrossEntropy));
    CHECK(report.before.topOneAccuracy >= 0.0 && report.before.topOneAccuracy <= 1.0);
    CHECK(report.after.topOneAccuracy >= 0.0 && report.after.topOneAccuracy <= 1.0);
    CHECK(report.trace[0].evaluated && report.trace[1].evaluated);
    CHECK(report.trace[0].trainingSteps == 2U &&
          report.trace[1].trainingSteps == 2U);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(decoder, checkpointAfter,
          checkpointSize, &afterLength) == UMI_STATUS_OK);
    CHECK(beforeLength == afterLength &&
          memcmp(checkpointBefore, checkpointAfter, beforeLength) == 0);
    CHECK(UmiAiTrainingSupervisedReceiptEncode(&report, receipt,
          sizeof(receipt), &written) == UMI_STATUS_OK);
    CHECK(written == sizeof(receipt));
    CHECK(UmiAiTrainingSupervisedReceiptDecode(receipt,
          written, &parsed) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedReceiptVerifyLive(&parsed, dataset,
          adapter) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderProjectionAdapterInspect(adapter,
          &current) == UMI_STATUS_OK);
    CHECK(current.completedAdapterSteps == report.finalAdapterSteps);
    UmiAiTrainingDecoderProjectionAdapterDestroy(adapter);
    UmiAiTrainingDecoderDestroy(decoder);
    UmiAiTrainingSupervisedDatasetDestroy(dataset);
    free(checkpointAfter);
    free(checkpointBefore);
    printf("Supervised native experiment: 4 adapter-only steps; held-out CE %.5f -> %.5f; frozen decoder unchanged\n",
           report.before.meanCrossEntropy, report.after.meanCrossEntropy);
    return 0;
}
