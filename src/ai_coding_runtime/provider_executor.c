/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/provider_executor.c
 *
 * PURPOSE:
 *   Execute provider-neutral coding requests through the existing AI runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/provider_executor.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_ai_coding_provider_execute(
    UmiAiRuntime *runtime,
    const UmiAiCodingRuntimeConfig *config,
    int approved,
    const UmiAiRequest *request,
    UmiAiResponse *out_response,
    UmiAiCodingProviderExecution *out_execution)
{
    UmiStatus status;

    if (runtime == NULL || config == NULL || request == NULL ||
        out_response == NULL || out_execution == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_execution, 0, sizeof(*out_execution));
    umi_ai_response_init(out_response);

    status = umi_ai_runtime_generate(
        runtime,
        config->provider_id,
        approved,
        request,
        out_response);

    (void)snprintf(
        out_execution->provider_id,
        sizeof(out_execution->provider_id),
        "%s",
        config->provider_id);
    (void)snprintf(
        out_execution->model_id,
        sizeof(out_execution->model_id),
        "%s",
        config->model_id);
    out_execution->status = status;
    out_execution->finish_reason = out_response->finish_reason;
    out_execution->usage = out_response->usage;
    out_execution->revision = 1U;
    return status;
}
