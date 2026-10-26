/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_receipt.c
 * PURPOSE: Check strict portable metadata round-trip, SHA-256 integrity,
 *          unknown format rejection and output preservation after corruption.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/supervised_receipt.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "receipt:%d %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    UmiAiTrainingSupervisedExperimentReport report = {0};
    UmiAiTrainingSupervisedReceiptInfo restored, preserved;
    uint8_t encoded[UMI_AI_SUPERVISED_RECEIPT_BYTES] = {0};
    uint8_t altered[UMI_AI_SUPERVISED_RECEIPT_BYTES] = {0};
    size_t written = 888U;
    static const char digest[65] =
        "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
    report.dataset.contextLength = 8U;
    report.dataset.trainingExamples = 2U;
    report.dataset.evaluationExamples = 2U;
    report.dataset.trainingTargets = 6U;
    report.dataset.evaluationTargets = 6U;
    for (size_t i = 0U; i < 32U; ++i)
        report.dataset.orderedCorpusSha256[i] = (uint8_t)i;
    memcpy(report.frozenCheckpointSha256, digest, sizeof(digest));
    report.initialAdapterSteps = 10U;
    report.finalAdapterSteps = 14U;
    report.completedTrainingSteps = 4U;
    report.completedEpochs = 2U;
    report.plan.epochs = 2U;
    report.plan.evaluateEveryEpochs = 1U;
    report.plan.shuffleSeed = UINT64_C(300);
    report.plan.learningRate = 0.05F;
    report.before.meanCrossEntropy = 4.3;
    report.after.meanCrossEntropy = 4.1;
    report.before.topOneAccuracy = 0.2;
    report.after.topOneAccuracy = 0.3;
    CHECK(UmiAiTrainingSupervisedReceiptEncode(&report, encoded, sizeof(encoded),
          &written) == UMI_STATUS_OK);
    CHECK(written == sizeof(encoded));
    CHECK(UmiAiTrainingSupervisedReceiptDecode(encoded, written,
          &restored) == UMI_STATUS_OK);
    CHECK(restored.version == 1U && restored.completedEpochs == 2U);
    CHECK(restored.initialAdapterSteps == 10U && restored.finalAdapterSteps == 14U);
    CHECK(restored.finalHeldOutLoss == 4.1);
    CHECK(restored.finalHeldOutAccuracy == 0.3);
    CHECK(restored.trainingTargets == 6U && restored.evaluationTargets == 6U);
    /* A failed parse does not overwrite the caller's prior valid evidence. */
    preserved = restored;
    memcpy(altered, encoded, sizeof(altered));
    altered[75] ^= 0x20U;
    CHECK(UmiAiTrainingSupervisedReceiptDecode(altered, sizeof(altered),
          &restored) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&preserved, &restored, sizeof(restored)) == 0);
    CHECK(UmiAiTrainingSupervisedReceiptDecode(encoded, sizeof(encoded) - 1U,
          &restored) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&preserved, &restored, sizeof(restored)) == 0);
    encoded[8] = 2U; /* Unsupported version AND changed digest. */
    CHECK(UmiAiTrainingSupervisedReceiptDecode(encoded, sizeof(encoded),
          &restored) == UMI_STATUS_PARSE_ERROR);
    report.before.meanCrossEntropy = -1.0;
    size_t untouched = 999U;
    CHECK(UmiAiTrainingSupervisedReceiptEncode(&report, altered,
          sizeof(altered), &untouched) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(untouched == 999U);
    puts("Portable unsigned receipt: roundtrip, tamper checks and atomic outputs passed");
    return 0;
}
