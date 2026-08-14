/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/agent_runtime_v2.c
 * PURPOSE: Implement the bounded Helix v2 specialist-agent registry.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/agent_runtime_v2.h"

#include <stdio.h>
#include <string.h>

void umi_helix_agent_runtime_v2_init(UmiHelixAgentRuntimeV2 *runtime)
{
    if (runtime != NULL) (void)memset(runtime, 0, sizeof(*runtime));
}

UmiStatus umi_helix_agent_runtime_v2_register_defaults(
    UmiHelixAgentRuntimeV2 *runtime)
{
    static const UmiHelixAgentRoleV2 roles[UMI_HELIX_V2_MAX_AGENTS] = {
        UMI_HELIX_AGENT_ROLE_DESIGNER,
        UMI_HELIX_AGENT_ROLE_BUILDER,
        UMI_HELIX_AGENT_ROLE_SUGGESTION,
        UMI_HELIX_AGENT_ROLE_REVIEWER,
        UMI_HELIX_AGENT_ROLE_TEST
    };
    static const uint32_t capabilities[UMI_HELIX_V2_MAX_AGENTS] = {
        UMI_HELIX_CAPABILITY_PLAN,
        UMI_HELIX_CAPABILITY_BUILD,
        UMI_HELIX_CAPABILITY_SUGGEST,
        UMI_HELIX_CAPABILITY_REVIEW,
        UMI_HELIX_CAPABILITY_TEST
    };
    size_t index;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_helix_agent_runtime_v2_init(runtime);
    for (index = 0U; index < UMI_HELIX_V2_MAX_AGENTS; ++index) {
        UmiHelixAgentV2 *agent = &runtime->agents[index];
        int written = snprintf(agent->agent_id, sizeof(agent->agent_id),
                               "helix.v2.%s",
                               umi_helix_agent_role_v2_text(roles[index]));
        if (written < 0 || (size_t)written >= sizeof(agent->agent_id)) {
            umi_helix_agent_runtime_v2_init(runtime);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        agent->role = roles[index];
        agent->capabilities = capabilities[index];
        agent->enabled = 1;
        runtime->count++;
    }
    return UMI_STATUS_OK;
}

const UmiHelixAgentV2 *umi_helix_agent_runtime_v2_find(
    const UmiHelixAgentRuntimeV2 *runtime,
    UmiHelixAgentRoleV2 role)
{
    size_t index;
    if (runtime == NULL) return NULL;
    for (index = 0U; index < runtime->count; ++index) {
        if (runtime->agents[index].role == role) return &runtime->agents[index];
    }
    return NULL;
}

int umi_helix_agent_v2_has_capability(const UmiHelixAgentV2 *agent,
                                      uint32_t capability)
{
    return agent != NULL && agent->enabled &&
           (agent->capabilities & capability) == capability;
}
