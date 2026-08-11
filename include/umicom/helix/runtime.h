/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/runtime.h
 *
 * PURPOSE:
 *   Own the Helix agent registry, memory, policy and current workflow.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The runtime coordinates bounded services; it does not itself edit source files or invoke models without going through declared agents and AI providers.
 */

#ifndef INCLUDE_UMICOM_HELIX_RUNTIME_H
#define INCLUDE_UMICOM_HELIX_RUNTIME_H

#include "umicom/helix/agent_registry.h"
#include "umicom/helix/memory.h"
#include "umicom/helix/policy.h"
#include "umicom/helix/workflow.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixRuntime {
    UmiHelixAgentRegistry agents;
    UmiHelixMemory memory;
    UmiHelixPolicy policy;
    UmiHelixWorkflow workflow;
} UmiHelixRuntime;

void umi_helix_runtime_init(UmiHelixRuntime *runtime, const char *workflow_id);
UmiStatus umi_helix_runtime_register_default_agents(UmiHelixRuntime *runtime,
                                                     const char *provider_id);

#ifdef __cplusplus
}
#endif

#endif
