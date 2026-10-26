/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/supervised_receipt.c
 * PURPOSE:
 *   Portable, integrity-checked, unsigned evidence for a completed approved
 *   supervised experiment, without publishing training content or model keys.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/supervised_receipt.h"
#include "umicom/base/sha256.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

static const unsigned char RECEIPT_MAGIC[8] = {
    'U', 'M', 'I', 'S', 'F', 'T', 'E', '1'
};

static void ReceiptPut32(uint8_t *destination, uint32_t value)
{
    for (size_t i = 0U; i < 4U; ++i)
        destination[i] = (uint8_t)(value >> (8U * i));
}

static void ReceiptPut64(uint8_t *destination, uint64_t value)
{
    for (size_t i = 0U; i < 8U; ++i)
        destination[i] = (uint8_t)(value >> (8U * i));
}

static uint32_t ReceiptGet32(const uint8_t *source)
{
    uint32_t value = 0U;
    for (size_t i = 0U; i < 4U; ++i)
        value |= (uint32_t)source[i] << (8U * i);
    return value;
}

static uint64_t ReceiptGet64(const uint8_t *source)
{
    uint64_t value = 0U;
    for (size_t i = 0U; i < 8U; ++i)
        value |= (uint64_t)source[i] << (8U * i);
    return value;
}

static int ReceiptFloatFormat(void)
{
    return sizeof(float) == 4U && FLT_RADIX == 2 && FLT_MANT_DIG == 24 &&
           FLT_MAX_EXP == 128 && sizeof(double) == 8U &&
           DBL_MANT_DIG == 53 && DBL_MAX_EXP == 1024;
}

static void ReceiptPutDouble(uint8_t *destination, double value)
{
    uint64_t bits = 0U;
    memcpy(&bits, &value, sizeof(bits));
    ReceiptPut64(destination, bits);
}

static double ReceiptGetDouble(const uint8_t *source)
{
    uint64_t bits = ReceiptGet64(source);
    double value = 0.0;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

static int ReceiptHexValue(unsigned char c)
{
    if (c >= '0' && c <= '9') return (int)(c - '0');
    if (c >= 'a' && c <= 'f') return (int)(c - 'a') + 10;
    return -1;
}

static int ReceiptReadHex(const char *hex, uint8_t digest[32])
{
    if (hex == NULL) return 0;
    for (size_t i = 0U; i < 32U; ++i) {
        int high = ReceiptHexValue((unsigned char)hex[2U * i]);
        int low = ReceiptHexValue((unsigned char)hex[2U * i + 1U]);
        if (high < 0 || low < 0) return 0;
        digest[i] = (uint8_t)((high << 4) | low);
    }
    return hex[64] == '\0';
}

static UmiStatus ReceiptDigest(const uint8_t *bytes, size_t count, uint8_t digest[32])
{
    UmiSha256 sha;
    UmiStatus status;
    UmiSha256Init(&sha);
    status = UmiSha256Update(&sha, bytes, count);
    if (status != UMI_STATUS_OK) return status;
    return UmiSha256Final(&sha, digest);
}

static int ReceiptInformationValid(const UmiAiTrainingSupervisedReceiptInfo *info)
{
    if (info->version != 1U ||
        (info->stoppedAtTarget != 0 && info->stoppedAtTarget != 1) ||
        info->completedEpochs == 0U ||
        info->completedEpochs > UMI_AI_SUPERVISED_EXPERIMENT_MAX_EPOCHS ||
        info->trainingExamples == 0U || info->evaluationExamples == 0U ||
        info->trainingExamples > UMI_AI_SUPERVISED_DATASET_MAX_EXAMPLES ||
        info->evaluationExamples > UMI_AI_SUPERVISED_DATASET_MAX_EXAMPLES ||
        info->contextLength == 0U ||
        info->contextLength > UMI_AI_SUPERVISED_DATASET_MAX_CONTEXT ||
        info->trainingTargets == 0U || info->evaluationTargets == 0U ||
        info->trainingTargets > info->trainingExamples * info->contextLength ||
        info->evaluationTargets > info->evaluationExamples * info->contextLength ||
        info->finalAdapterSteps < info->initialAdapterSteps ||
        info->finalAdapterSteps - info->initialAdapterSteps !=
            (uint64_t)(info->completedEpochs * info->trainingExamples) ||
        !isfinite(info->initialHeldOutLoss) || info->initialHeldOutLoss < 0.0 ||
        !isfinite(info->finalHeldOutLoss) || info->finalHeldOutLoss < 0.0 ||
        !isfinite(info->initialHeldOutAccuracy) ||
        info->initialHeldOutAccuracy < 0.0 || info->initialHeldOutAccuracy > 1.0 ||
        !isfinite(info->finalHeldOutAccuracy) ||
        info->finalHeldOutAccuracy < 0.0 || info->finalHeldOutAccuracy > 1.0 ||
        !isfinite(info->learningRate) || info->learningRate <= 0.0F)
        return 0;
    return 1;
}

UmiStatus UmiAiTrainingSupervisedReceiptEncode(
    const UmiAiTrainingSupervisedExperimentReport *report,
    uint8_t *outBytes, size_t capacity, size_t *outWritten)
{
    UmiAiTrainingSupervisedReceiptInfo info = {0};
    uint8_t record[UMI_AI_SUPERVISED_RECEIPT_BYTES] = {0};
    uint32_t floatBits = 0U;
    UmiStatus status;
    if (report == NULL || outBytes == NULL || outWritten == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!ReceiptFloatFormat()) return UMI_STATUS_UNAVAILABLE;
    if (capacity < sizeof(record)) return UMI_STATUS_CAPACITY_EXCEEDED;
    info.version = 1U;
    info.stoppedAtTarget = report->stoppedAtTarget;
    info.completedEpochs = report->completedEpochs;
    info.trainingExamples = report->dataset.trainingExamples;
    info.evaluationExamples = report->dataset.evaluationExamples;
    info.contextLength = report->dataset.contextLength;
    info.initialAdapterSteps = report->initialAdapterSteps;
    info.finalAdapterSteps = report->finalAdapterSteps;
    info.trainingTargets = report->dataset.trainingTargets;
    info.evaluationTargets = report->dataset.evaluationTargets;
    info.initialHeldOutLoss = report->before.meanCrossEntropy;
    info.finalHeldOutLoss = report->after.meanCrossEntropy;
    info.initialHeldOutAccuracy = report->before.topOneAccuracy;
    info.finalHeldOutAccuracy = report->after.topOneAccuracy;
    info.shuffleSeed = report->plan.shuffleSeed;
    info.learningRate = report->plan.learningRate;
    memcpy(info.orderedCorpusSha256, report->dataset.orderedCorpusSha256, 32U);
    if (!ReceiptReadHex(report->frozenCheckpointSha256,
                        info.frozenCheckpointSha256) ||
        !ReceiptInformationValid(&info) ||
        UmiAiTrainingSupervisedExperimentPlanValidate(&report->plan) != UMI_STATUS_OK ||
        (report->stoppedAtTarget != 0 &&
            (report->plan.stopAtEvaluationLoss == 0.0 ||
             report->after.meanCrossEntropy > report->plan.stopAtEvaluationLoss)) ||
        report->completedTrainingSteps !=
            report->completedEpochs * report->dataset.trainingExamples ||
        report->plan.epochs < report->completedEpochs)
        return UMI_STATUS_INVALID_ARGUMENT;

    memcpy(record, RECEIPT_MAGIC, sizeof(RECEIPT_MAGIC));
    ReceiptPut32(record + 8U, info.version);
    ReceiptPut32(record + 12U, (uint32_t)info.stoppedAtTarget);
    ReceiptPut32(record + 16U, (uint32_t)info.completedEpochs);
    ReceiptPut32(record + 20U, (uint32_t)info.trainingExamples);
    ReceiptPut32(record + 24U, (uint32_t)info.evaluationExamples);
    ReceiptPut32(record + 28U, (uint32_t)info.contextLength);
    ReceiptPut64(record + 32U, info.initialAdapterSteps);
    ReceiptPut64(record + 40U, info.finalAdapterSteps);
    ReceiptPut64(record + 48U, (uint64_t)info.trainingTargets);
    ReceiptPut64(record + 56U, (uint64_t)info.evaluationTargets);
    ReceiptPutDouble(record + 64U, info.initialHeldOutLoss);
    ReceiptPutDouble(record + 72U, info.finalHeldOutLoss);
    ReceiptPutDouble(record + 80U, info.initialHeldOutAccuracy);
    ReceiptPutDouble(record + 88U, info.finalHeldOutAccuracy);
    ReceiptPut64(record + 96U, info.shuffleSeed);
    memcpy(&floatBits, &info.learningRate, sizeof(floatBits));
    ReceiptPut32(record + 104U, floatBits);
    /* 108..111 reserved-zero for future compatible additions. */
    memcpy(record + 112U, info.orderedCorpusSha256, 32U);
    memcpy(record + 144U, info.frozenCheckpointSha256, 32U);
    status = ReceiptDigest(record, 176U, record + 176U);
    if (status != UMI_STATUS_OK) return status;
    memcpy(outBytes, record, sizeof(record));
    *outWritten = sizeof(record);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSupervisedReceiptDecode(
    const uint8_t *bytes, size_t length,
    UmiAiTrainingSupervisedReceiptInfo *outInfo)
{
    UmiAiTrainingSupervisedReceiptInfo info = {0};
    uint8_t digest[32] = {0};
    uint32_t floatBits;
    UmiStatus status;
    if (bytes == NULL || outInfo == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!ReceiptFloatFormat()) return UMI_STATUS_UNAVAILABLE;
    if (length != UMI_AI_SUPERVISED_RECEIPT_BYTES ||
        memcmp(bytes, RECEIPT_MAGIC, sizeof(RECEIPT_MAGIC)) != 0 ||
        ReceiptGet32(bytes + 108U) != 0U)
        return UMI_STATUS_PARSE_ERROR;
    status = ReceiptDigest(bytes, 176U, digest);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(digest, bytes + 176U, sizeof(digest)) != 0)
        return UMI_STATUS_PARSE_ERROR;
    /* Check unsigned wire values BEFORE any narrowing conversions. */
    if (ReceiptGet32(bytes + 12U) > 1U ||
        ReceiptGet64(bytes + 48U) > SIZE_MAX ||
        ReceiptGet64(bytes + 56U) > SIZE_MAX)
        return UMI_STATUS_PARSE_ERROR;
    info.version = ReceiptGet32(bytes + 8U);
    info.stoppedAtTarget = (int)ReceiptGet32(bytes + 12U);
    info.completedEpochs = (size_t)ReceiptGet32(bytes + 16U);
    info.trainingExamples = (size_t)ReceiptGet32(bytes + 20U);
    info.evaluationExamples = (size_t)ReceiptGet32(bytes + 24U);
    info.contextLength = (size_t)ReceiptGet32(bytes + 28U);
    info.initialAdapterSteps = ReceiptGet64(bytes + 32U);
    info.finalAdapterSteps = ReceiptGet64(bytes + 40U);
    info.trainingTargets = (size_t)ReceiptGet64(bytes + 48U);
    info.evaluationTargets = (size_t)ReceiptGet64(bytes + 56U);
    info.initialHeldOutLoss = ReceiptGetDouble(bytes + 64U);
    info.finalHeldOutLoss = ReceiptGetDouble(bytes + 72U);
    info.initialHeldOutAccuracy = ReceiptGetDouble(bytes + 80U);
    info.finalHeldOutAccuracy = ReceiptGetDouble(bytes + 88U);
    info.shuffleSeed = ReceiptGet64(bytes + 96U);
    floatBits = ReceiptGet32(bytes + 104U);
    memcpy(&info.learningRate, &floatBits, sizeof(info.learningRate));
    memcpy(info.orderedCorpusSha256, bytes + 112U, 32U);
    memcpy(info.frozenCheckpointSha256, bytes + 144U, 32U);
    if (!ReceiptInformationValid(&info)) return UMI_STATUS_PARSE_ERROR;
    *outInfo = info;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSupervisedReceiptVerifyLive(
    const UmiAiTrainingSupervisedReceiptInfo *receipt,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingDecoderProjectionAdapter *adapter)
{
    UmiAiTrainingSupervisedDatasetInfo currentData = {0};
    UmiAiTrainingDecoderProjectionAdapterInfo currentAdapter = {0};
    uint8_t observedBase[32];
    UmiStatus status;
    if (receipt == NULL || dataset == NULL || adapter == NULL ||
        !ReceiptInformationValid(receipt))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingSupervisedDatasetInspect(dataset, &currentData);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingDecoderProjectionAdapterInspect(adapter, &currentAdapter);
    if (status != UMI_STATUS_OK) return status;
    if (!ReceiptReadHex(currentAdapter.frozenCheckpointSha256, observedBase))
        return UMI_STATUS_INVALID_STATE;
    return currentData.contextLength == receipt->contextLength &&
           currentData.trainingExamples == receipt->trainingExamples &&
           currentData.evaluationExamples == receipt->evaluationExamples &&
           currentData.trainingTargets == receipt->trainingTargets &&
           currentData.evaluationTargets == receipt->evaluationTargets &&
           memcmp(currentData.orderedCorpusSha256,
                  receipt->orderedCorpusSha256, 32U) == 0 &&
           memcmp(observedBase, receipt->frozenCheckpointSha256, 32U) == 0 &&
           currentAdapter.completedAdapterSteps == receipt->finalAdapterSteps
           ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
