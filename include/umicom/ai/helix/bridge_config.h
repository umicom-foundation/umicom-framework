/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/helix/bridge_config.h
 *
 * PURPOSE:
 *   Configure which existing AI provider may reason for bounded Helix agents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_AI_HELIX_BRIDGE_CONFIG_H
#define INCLUDE_UMICOM_AI_HELIX_BRIDGE_CONFIG_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/helix/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiHelixBridgeConfig {
    char provider_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    uint32_t max_output_tokens;
    double temperature;
    uint32_t allowed_actions;
    size_t max_context_bytes;
    int allow_source_control;
    int strict_protocol;
} UmiAiHelixBridgeConfig;

UmiStatus umi_ai_helix_bridge_config_init(UmiAiHelixBridgeConfig *config,
                                          const char *provider_id,
                                          const char *model_id);
UmiStatus umi_ai_helix_bridge_config_validate(
    const UmiAiHelixBridgeConfig *config);

#ifdef __cplusplus
}
#endif

#endif
