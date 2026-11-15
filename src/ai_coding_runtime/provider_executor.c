/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/provider_executor.c
 *
 * PURPOSE:
 *   Execute provider-neutral coding requests through the existing AI runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/provider_executor.h"

#include <stdio.h>
#include <string.h>

/*
 * Perform ai coding provider through the module contract so client applications do not
 * duplicate its policy.
 */
/* Framework checked generation admits complete request/config values before formatting selection and publishes a response only on validated provider success. Previous partial response and unchecked config handling are retained for owner review. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_provider_execute(
    UmiAiRuntime *runtime,
    const UmiAiCodingRuntimeConfig *config,
    int approved,
    const UmiAiRequest *request,
    UmiAiResponse *out_response,
    UmiAiCodingProviderExecution *out_execution)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

#endif
UmiStatus umi_ai_coding_provider_execute(
    UmiAiRuntime *runtime,
    const UmiAiCodingRuntimeConfig *config,
    int approved,
    const UmiAiRequest *request,
    UmiAiResponse *out_response,
    UmiAiCodingProviderExecution *out_execution)
{
    if (runtime == NULL || config == NULL || request == NULL ||
        out_response == NULL || out_execution == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_ai_coding_runtime_config_validate(config);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiRequestValidate(request);
    if (status != UMI_STATUS_OK) return status;

    /* Capture validated selection before calling external code. The response
     * remains unchanged on failure; execution metadata describes this attempt
     * rather than borrowing finish/usage fields from an earlier response. */
    UmiAiCodingProviderExecution execution = {0};
    memcpy(execution.provider_id, config->provider_id, sizeof(execution.provider_id));
    memcpy(execution.model_id, config->model_id, sizeof(execution.model_id));
    UmiAiGenerationReport report;
    status = UmiAiRuntimeGenerateChecked(runtime, execution.provider_id, approved,
        request, out_response, &report);
    execution.status = status;
    if (report.response_published) {
        execution.finish_reason = out_response->finish_reason;
        execution.usage = out_response->usage;
    }
    execution.revision = 1U;
    *out_execution = execution;
    return status;
}