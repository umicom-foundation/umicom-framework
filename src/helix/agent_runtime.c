/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/agent_runtime.c
 * PURPOSE: Implement the bounded Helix v2 specialist-agent registry.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/agent_runtime.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise helix agent runtime from caller-provided values so later operations receive a
 * known state.
 */
void umi_helix_agent_runtime_init(UmiHelixAgentRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime != NULL) (void)memset(runtime, 0, sizeof(*runtime));
}

/*
 * Provide the helix agent runtime register defaults operation used by this module and its
 * client applications.
 */
UmiStatus umi_helix_agent_runtime_register_defaults(
    UmiHelixAgentRuntime *runtime)
{
    static const UmiHelixAgentRole roles[UMI_HELIX_RUNTIME_MAX_AGENTS] = {
        UMI_HELIX_AGENT_ROLE_DESIGNER,
        UMI_HELIX_AGENT_ROLE_BUILDER,
        UMI_HELIX_AGENT_ROLE_SUGGESTION,
        UMI_HELIX_AGENT_ROLE_REVIEWER,
        UMI_HELIX_AGENT_ROLE_TEST
    };
    static const uint32_t capabilities[UMI_HELIX_RUNTIME_MAX_AGENTS] = {
        UMI_HELIX_CAPABILITY_PLAN,
        UMI_HELIX_CAPABILITY_BUILD,
        UMI_HELIX_CAPABILITY_SUGGEST,
        UMI_HELIX_CAPABILITY_REVIEW,
        UMI_HELIX_CAPABILITY_TEST
    };
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_helix_agent_runtime_init(runtime);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_HELIX_RUNTIME_MAX_AGENTS; ++index) {
        UmiHelixRuntimeAgent *agent = &runtime->agents[index];
        int written = snprintf(agent->agent_id, sizeof(agent->agent_id),
                               "helix.v2.%s",
                               umi_helix_agent_role_text(roles[index]));
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(agent->agent_id)) {
            umi_helix_agent_runtime_init(runtime);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        agent->role = roles[index];
        agent->capabilities = capabilities[index];
        agent->enabled = 1;
        runtime->count++;
    }
    return UMI_STATUS_OK;
}

/*
 * Find helix agent runtime while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiHelixRuntimeAgent *umi_helix_agent_runtime_find(
    const UmiHelixAgentRuntime *runtime,
    UmiHelixAgentRole role)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (runtime->agents[index].role == role) return &runtime->agents[index];
    }
    return NULL;
}

/*
 * Provide the helix runtime agent has capability operation used by this module and its
 * client applications.
 */
int umi_helix_runtime_agent_has_capability(const UmiHelixRuntimeAgent *agent,
                                      uint32_t capability)
{
    return agent != NULL && agent->enabled &&
           (agent->capabilities & capability) == capability;
}
