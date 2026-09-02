/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/runtime.c
 *
 * PURPOSE:
 *   Initialise the Helix runtime and its default specialist agents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Default agents cover design, build, suggestion, diagnostics, security and release while keeping each permission distinct.
 */

#include "umicom/helix/runtime.h"
#include <stddef.h>

/*
 * Initialise helix runtime from caller-provided values so later operations receive a known
 * state.
 */
void umi_helix_runtime_init(UmiHelixRuntime *runtime, const char *workflow_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime != NULL) {
        umi_helix_agent_registry_init(&runtime->agents);
        umi_helix_memory_init(&runtime->memory);
        runtime->policy = umi_helix_policy_default();
        umi_helix_workflow_init(&runtime->workflow, workflow_id);
    }
}

/*
 * Provide the helix runtime register default agents operation used by this module and its
 * client applications.
 */
UmiStatus umi_helix_runtime_register_default_agents(UmiHelixRuntime *runtime,
                                                     const char *provider_id)
{
    static const UmiHelixAgentKind kinds[] = {
        UMI_HELIX_AGENT_DESIGNER,
        UMI_HELIX_AGENT_BUILDER,
        UMI_HELIX_AGENT_SUGGESTION,
        UMI_HELIX_AGENT_DIAGNOSTIC,
        UMI_HELIX_AGENT_SECURITY,
        UMI_HELIX_AGENT_RELEASE
    };
    static const char *ids[] = {
        "helix.designer", "helix.builder", "helix.suggestion",
        "helix.diagnostic", "helix.security", "helix.release"
    };
    static const char *permissions[] = {
        "source.propose", "build.execute", "source.suggest",
        "diagnostics.read", "security.inspect", "release.propose"
    };
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || provider_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < sizeof(kinds) / sizeof(kinds[0]); ++i) {
        UmiHelixAgent agent;
        UmiStatus status = umi_helix_agent_init(&agent, ids[i], kinds[i],
                                                provider_id, permissions[i]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_helix_agent_registry_add(&runtime->agents, &agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
