/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/helix/bridge.h
 *
 * PURPOSE:
 *   Connect the existing policy-aware AI runtime to the existing governed Helix agent model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_AI_HELIX_BRIDGE_H
#define INCLUDE_UMICOM_AI_HELIX_BRIDGE_H

#include "umicom/ai/helix/bridge_config.h"
#include "umicom/ai/helix/types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct UmiAiRuntime;
typedef struct UmiAiRuntime UmiAiRuntime;

typedef struct UmiAiHelixBridge {
    UmiAiRuntime *runtime;
    UmiAiHelixBridgeConfig config;
} UmiAiHelixBridge;

UmiStatus umi_ai_helix_bridge_init(UmiAiHelixBridge *bridge,
                                   UmiAiRuntime *runtime,
                                   const UmiAiHelixBridgeConfig *config);
UmiStatus umi_ai_helix_bridge_run(UmiAiHelixBridge *bridge,
                                  const UmiAiHelixAgentRequest *request,
                                  UmiAiHelixAgentResult *result);

#ifdef __cplusplus
}
#endif

#endif
