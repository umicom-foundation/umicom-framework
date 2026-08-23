/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/config.c
 *
 * PURPOSE:
 *   Implement conservative defaults and validation for AI coding execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/config.h"

#include <string.h>

void umi_ai_coding_runtime_config_init(UmiAiCodingRuntimeConfig *config)
{
    if (config == NULL) return;

    (void)memset(config, 0, sizeof(*config));
    config->provider_kind = UMI_AI_PROVIDER_LOCAL;
    config->context_token_budget = 24000U;
    config->max_output_tokens = 8192U;
    config->maximum_iterations = 3U;
    config->maximum_context_files = 12U;
    config->temperature = 0.15;
    config->allow_tools = 0;
    config->auto_apply_approved_patch = 0;
    config->auto_approve = 0;
    config->rollback_on_validation_failure = 1;
    config->require_validation = 1;
    config->allow_sensitive_context = 0;
}

UmiStatus umi_ai_coding_runtime_config_validate(
    const UmiAiCodingRuntimeConfig *config)
{
    if (config == NULL ||
        config->provider_id[0] == '\0' ||
        config->model_id[0] == '\0' ||
        config->context_token_budget == 0U ||
        config->max_output_tokens == 0U ||
        config->maximum_iterations == 0U ||
        config->maximum_iterations > UMI_AI_CODING_RUNTIME_MAX_ITERATIONS ||
        config->maximum_context_files == 0U ||
        config->maximum_context_files > UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY ||
        config->temperature < 0.0 ||
        config->temperature > 2.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
