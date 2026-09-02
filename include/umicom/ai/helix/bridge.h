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
/**
 * Represent the ai runtime data shared with callers of this public contract.
 */
typedef struct UmiAiRuntime UmiAiRuntime;

/**
 * Represent the ai helix bridge data shared with callers of this public contract.
 */
typedef struct UmiAiHelixBridge {
    UmiAiRuntime *runtime;
    UmiAiHelixBridgeConfig config;
} UmiAiHelixBridge;

/**
 * Initialise ai helix bridge from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_helix_bridge_init(UmiAiHelixBridge *bridge,
                                   UmiAiRuntime *runtime,
                                   const UmiAiHelixBridgeConfig *config);
/**
 * Perform ai helix bridge through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ai_helix_bridge_run(UmiAiHelixBridge *bridge,
                                  const UmiAiHelixAgentRequest *request,
                                  UmiAiHelixAgentResult *result);

#ifdef __cplusplus
}
#endif

#endif
