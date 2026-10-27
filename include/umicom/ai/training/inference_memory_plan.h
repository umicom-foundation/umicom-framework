/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/inference_memory_plan.h
 *
 * PURPOSE:
 *   Estimate bounded multi-session key/value allocation before inference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_INFERENCE_MEMORY_PLAN_H
#define UMICOM_AI_TRAINING_INFERENCE_MEMORY_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingInferenceMemoryRequest {
    size_t layers;
    size_t contextTokens;
    size_t queryHeads;
    size_t keyValueHeads;
    size_t headChannels;
    size_t bytesPerScalar;
    size_t sessionCount;
    size_t budgetBytes;
} UmiAiTrainingInferenceMemoryRequest;

typedef struct UmiAiTrainingInferenceMemoryReport {
    size_t scalarsPerSession;
    size_t bytesPerSession;
    size_t totalBytes;
    size_t fullMultiheadTotalBytes;
    size_t reductionBytes;
    size_t admittedSessions;
    int fitsBudget;
} UmiAiTrainingInferenceMemoryReport;

/* Calculate raw K and V payload only: 2 * layers * context * KV heads *
 * headChannels * scalarBytes * sessions. All fields except budgetBytes must
 * be positive. Query heads must divide evenly by KV heads; this is a
 * capacity model, not a GQA inference implementation. If budgetBytes is
 * zero there is no ceiling. On rejection outReport is unchanged. */
UmiStatus UmiAiTrainingInferenceMemoryPlan(
    const UmiAiTrainingInferenceMemoryRequest *request,
    UmiAiTrainingInferenceMemoryReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_INFERENCE_MEMORY_PLAN_H */
