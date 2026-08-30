/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/agent_runtime.h
 * PURPOSE: Register the five bounded specialist agents used by Helix v2.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Each agent advertises a capability mask; a friendly name
 * never grants permission to perform a filesystem or source-control action. */
#ifndef INCLUDE_UMICOM_HELIX_AGENT_RUNTIME_H
#define INCLUDE_UMICOM_HELIX_AGENT_RUNTIME_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/execution_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_HELIX_CAPABILITY_PLAN (UINT32_C(1) << 0)
#define UMI_HELIX_CAPABILITY_SUGGEST (UINT32_C(1) << 1)
#define UMI_HELIX_CAPABILITY_BUILD (UINT32_C(1) << 2)
#define UMI_HELIX_CAPABILITY_TEST (UINT32_C(1) << 3)
#define UMI_HELIX_CAPABILITY_REVIEW (UINT32_C(1) << 4)

typedef struct UmiHelixRuntimeAgent {
    char agent_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixAgentRole role;
    uint32_t capabilities;
    int enabled;
} UmiHelixRuntimeAgent;

typedef struct UmiHelixAgentRuntime {
    UmiHelixRuntimeAgent agents[UMI_HELIX_RUNTIME_MAX_AGENTS];
    size_t count;
} UmiHelixAgentRuntime;

void umi_helix_agent_runtime_init(UmiHelixAgentRuntime *runtime);
UmiStatus umi_helix_agent_runtime_register_defaults(
    UmiHelixAgentRuntime *runtime);
const UmiHelixRuntimeAgent *umi_helix_agent_runtime_find(
    const UmiHelixAgentRuntime *runtime,
    UmiHelixAgentRole role);
int umi_helix_runtime_agent_has_capability(const UmiHelixRuntimeAgent *agent,
                                      uint32_t capability);

#ifdef __cplusplus
}
#endif

#endif
