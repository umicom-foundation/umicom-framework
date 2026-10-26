/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-supervised-experiment/training_demo.c
 * PURPOSE: Demonstrate a real bounded native C23 supervised experiment using
 *          synthetic approved training fixtures; display aggregate evidence
 *          without logging text, source IDs or personal information.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "supervised_fixture.h"
#include "umicom/ai/training/supervised_experiment.h"
#include "umicom/ai/training/supervised_receipt.h"
#include "umicom/base/sha256.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    UmiTestSupervisedFixture fixture;
    UmiAiTrainingSupervisedDataset *dataset = NULL;
    UmiAiTrainingDecoder *decoder = NULL;
    UmiAiTrainingDecoderProjectionAdapter *adapter = NULL;
    UmiAiTrainingSupervisedExperimentReport report = {0};
    UmiAiTrainingSupervisedReceiptInfo parsed = {0};
    uint8_t receipt[UMI_AI_SUPERVISED_RECEIPT_BYTES] = {0};
    size_t receiptBytes = 0U;
    char digest[UMI_SHA256_HEX_CAPACITY] = {0};
    UmiStatus status;
    if (argc == 2 &&
        (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "help") == 0)) {
        puts("Native synthetic supervised demonstration: run without arguments.");
        return 0;
    }
    if (argc != 1) {
        fputs("No file inputs or external paths are accepted by this teaching demo.\n",
              stderr);
        return 2;
    }
    const UmiAiTrainingDecoderConfig config = {
        260U, 8U, 8U, 16U, 2U, UINT64_C(502)
    };
    const UmiAiTrainingSupervisedExperimentPlan plan = {
        2U, 1U, UINT64_C(2026), 0.025F, 0.0
    };
    status = UmiTestSupervisedFixturePrepare(&fixture);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiAiTrainingSupervisedDatasetCreate(
        fixture.records, fixture.lengths, fixture.evidence,
        UMI_TEST_SUPERVISED_EXAMPLES, 8U, &dataset);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiAiTrainingDecoderCreate(&config, &decoder);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiAiTrainingDecoderProjectionAdapterCreate(decoder,
        UMI_AI_DECODER_ADAPT_QUERY | UMI_AI_DECODER_ADAPT_VALUE,
        2U, UINT64_C(101), &adapter);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiAiTrainingSupervisedExperimentRun(
        adapter, dataset, &plan, &report);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiAiTrainingSupervisedReceiptEncode(
        &report, receipt, sizeof(receipt), &receiptBytes);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiAiTrainingSupervisedReceiptDecode(
        receipt, receiptBytes, &parsed);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiAiTrainingSupervisedReceiptVerifyLive(&parsed, dataset, adapter);
    if (status != UMI_STATUS_OK) goto cleanup;
    UmiSha256Hex(receipt + UMI_AI_SUPERVISED_RECEIPT_BYTES - 32U, digest);
    printf("Synthetic reviewed sources: %zu (training %zu, held-out %zu)\n",
        report.dataset.distinctSources, report.dataset.trainingExamples,
        report.dataset.evaluationExamples);
    printf("Adapter SGD updates: %zu across %zu complete epochs\n",
        report.completedTrainingSteps, report.completedEpochs);
    printf("Held-out selected-token cross-entropy: %.6f -> %.6f\n",
        report.before.meanCrossEntropy, report.after.meanCrossEntropy);
    printf("Held-out selected-token top-1 accuracy: %.1f%% -> %.1f%%\n",
        report.before.topOneAccuracy * 100.0, report.after.topOneAccuracy * 100.0);
    printf("Unsigned evidence receipt digest: %s\n", digest);
    puts("Educational experiment only: no publisher signature, no production model qualification.");
cleanup:
    if (status != UMI_STATUS_OK)
        fprintf(stderr, "Native supervised experiment refused: %d\n", (int)status);
    UmiAiTrainingDecoderProjectionAdapterDestroy(adapter);
    UmiAiTrainingDecoderDestroy(decoder);
    UmiAiTrainingSupervisedDatasetDestroy(dataset);
    return status == UMI_STATUS_OK ? 0 : 1;
}
