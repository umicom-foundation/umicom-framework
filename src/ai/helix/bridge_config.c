/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/helix/bridge_config.c
 *
 * PURPOSE:
 *   Create and validate conservative AI-to-Helix bridge configuration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/helix/bridge_config.h"
#include <string.h>

static UmiStatus copy_text(char *output, size_t capacity, const char *text)
{
    size_t length;
    if (output == NULL || capacity == 0U || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(output, text, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_helix_bridge_config_init(UmiAiHelixBridgeConfig *config,
                                          const char *provider_id,
                                          const char *model_id)
{
    UmiStatus status;
    if (config == NULL || provider_id == NULL || model_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(config, 0, sizeof(*config));
    status = copy_text(config->provider_id, sizeof(config->provider_id), provider_id);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(config->model_id, sizeof(config->model_id), model_id);
    if (status != UMI_STATUS_OK) return status;
    config->max_output_tokens = 768U;
    config->temperature = 0.20;
    config->allowed_actions =
        umi_ai_helix_action_mask(UMI_HELIX_ACTION_READ) |
        umi_ai_helix_action_mask(UMI_HELIX_ACTION_FILESYSTEM) |
        umi_ai_helix_action_mask(UMI_HELIX_ACTION_BUILD) |
        umi_ai_helix_action_mask(UMI_HELIX_ACTION_TEST) |
        umi_ai_helix_action_mask(UMI_HELIX_ACTION_REVIEW);
    config->max_context_bytes = UMI_AI_HELIX_CONTEXT_CAPACITY - 1U;
    config->allow_source_control = 0;
    config->strict_protocol = 1;
    return umi_ai_helix_bridge_config_validate(config);
}

UmiStatus umi_ai_helix_bridge_config_validate(
    const UmiAiHelixBridgeConfig *config)
{
    if (config == NULL || config->provider_id[0] == '\0' ||
        config->model_id[0] == '\0' || config->max_output_tokens == 0U ||
        config->temperature < 0.0 || config->temperature > 2.0 ||
        config->allowed_actions == 0U || config->max_context_bytes == 0U ||
        config->max_context_bytes >= UMI_AI_HELIX_CONTEXT_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
