/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/agent_runtime_v2.h
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
#ifndef INCLUDE_UMICOM_HELIX_AGENT_RUNTIME_V2_H
#define INCLUDE_UMICOM_HELIX_AGENT_RUNTIME_V2_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/v2_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_HELIX_CAPABILITY_PLAN (UINT32_C(1) << 0)
#define UMI_HELIX_CAPABILITY_SUGGEST (UINT32_C(1) << 1)
#define UMI_HELIX_CAPABILITY_BUILD (UINT32_C(1) << 2)
#define UMI_HELIX_CAPABILITY_TEST (UINT32_C(1) << 3)
#define UMI_HELIX_CAPABILITY_REVIEW (UINT32_C(1) << 4)

typedef struct UmiHelixAgentV2 {
    char agent_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixAgentRoleV2 role;
    uint32_t capabilities;
    int enabled;
} UmiHelixAgentV2;

typedef struct UmiHelixAgentRuntimeV2 {
    UmiHelixAgentV2 agents[UMI_HELIX_V2_MAX_AGENTS];
    size_t count;
} UmiHelixAgentRuntimeV2;

void umi_helix_agent_runtime_v2_init(UmiHelixAgentRuntimeV2 *runtime);
UmiStatus umi_helix_agent_runtime_v2_register_defaults(
    UmiHelixAgentRuntimeV2 *runtime);
const UmiHelixAgentV2 *umi_helix_agent_runtime_v2_find(
    const UmiHelixAgentRuntimeV2 *runtime,
    UmiHelixAgentRoleV2 role);
int umi_helix_agent_v2_has_capability(const UmiHelixAgentV2 *agent,
                                      uint32_t capability);

#ifdef __cplusplus
}
#endif

#endif
