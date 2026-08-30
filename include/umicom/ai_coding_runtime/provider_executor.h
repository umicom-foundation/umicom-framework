/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/provider_executor.h
 *
 * PURPOSE:
 *   Execute one prepared coding request through the existing governed UmiAiRuntime
 *   provider registry and retain provider/model usage in a reusable snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_PROVIDER_EXECUTOR_H
#define UMICOM_AI_CODING_RUNTIME_PROVIDER_EXECUTOR_H
#include "umicom/ai_coding_runtime/prompt.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingProviderExecution {
    char provider_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    UmiStatus status;
    UmiAiFinishReason finish_reason;
    UmiAiUsage usage;
    uint64_t revision;
} UmiAiCodingProviderExecution;

UmiStatus umi_ai_coding_provider_execute(
    UmiAiRuntime *runtime,
    const UmiAiCodingRuntimeConfig *config,
    int approved,
    const UmiAiRequest *request,
    UmiAiResponse *out_response,
    UmiAiCodingProviderExecution *out_execution);

#ifdef __cplusplus
}
#endif
#endif
