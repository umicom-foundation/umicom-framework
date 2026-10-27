/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/inference_memory_plan.c
 *
 * PURPOSE:
 *   Calculate overflow-safe, multi-session raw K/V cache memory requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/inference_memory_plan.h"
#include <stdint.h>

static int CheckedMultiply(size_t a, size_t b, size_t *out)
{
    if (a != 0U && b > SIZE_MAX / a) return 0;
    *out = a * b;
    return 1;
}

UmiStatus UmiAiTrainingInferenceMemoryPlan(
    const UmiAiTrainingInferenceMemoryRequest *request,
    UmiAiTrainingInferenceMemoryReport *outReport)
{
    UmiAiTrainingInferenceMemoryReport result = {0};
    size_t count = 0U, full = 0U;
    if (request == NULL || outReport == NULL || request->layers == 0U ||
        request->contextTokens == 0U || request->queryHeads == 0U ||
        request->keyValueHeads == 0U || request->headChannels == 0U ||
        request->bytesPerScalar == 0U || request->sessionCount == 0U ||
        request->queryHeads % request->keyValueHeads != 0U ||
        request->queryHeads < request->keyValueHeads)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Include both K and V; reject overflow BEFORE publishing anything. */
    if (!CheckedMultiply(2U, request->layers, &count) ||
        !CheckedMultiply(count, request->contextTokens, &count) ||
        !CheckedMultiply(count, request->keyValueHeads, &count) ||
        !CheckedMultiply(count, request->headChannels, &count))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    result.scalarsPerSession = count;
    if (!CheckedMultiply(count, request->bytesPerScalar,
                         &result.bytesPerSession) ||
        !CheckedMultiply(result.bytesPerSession, request->sessionCount,
                         &result.totalBytes) ||
        !CheckedMultiply(result.totalBytes,
                         request->queryHeads / request->keyValueHeads, &full))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    result.fullMultiheadTotalBytes = full;
    result.reductionBytes = full - result.totalBytes;
    result.fitsBudget = request->budgetBytes == 0U ||
                        result.totalBytes <= request->budgetBytes;
    result.admittedSessions = request->budgetBytes == 0U ?
        request->sessionCount : request->budgetBytes / result.bytesPerSession;
    if (result.admittedSessions > request->sessionCount)
        result.admittedSessions = request->sessionCount;
    *outReport = result;
    return UMI_STATUS_OK;
}
