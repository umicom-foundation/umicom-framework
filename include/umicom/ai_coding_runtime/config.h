/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/config.h
 *
 * PURPOSE:
 *   Configure provider/model selection, approval policy, iteration limits and
 *   validation behavior for one operational coding-agent runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_CONFIG_H
#define UMICOM_AI_CODING_RUNTIME_CONFIG_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding runtime config data shared with callers of this public contract.
 */
typedef struct UmiAiCodingRuntimeConfig {
    char provider_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    UmiAiProviderKind provider_kind;
    uint32_t context_token_budget;
    uint32_t max_output_tokens;
    uint32_t maximum_iterations;
    size_t maximum_context_files;
    double temperature;
    int allow_tools;
    int auto_apply_approved_patch;
    int auto_approve;
    int rollback_on_validation_failure;
    int require_validation;
    int allow_sensitive_context;
} UmiAiCodingRuntimeConfig;

/**
 * Initialise ai coding runtime config from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_runtime_config_init(UmiAiCodingRuntimeConfig *config);
/**
 * Check that ai coding runtime config satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ai_coding_runtime_config_validate(
    const UmiAiCodingRuntimeConfig *config);

#ifdef __cplusplus
}
#endif
#endif
