/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/agent.c
 *
 * PURPOSE:
 *   Initialise bounded Helix specialist-agent descriptors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The descriptor is small enough to copy into registries while provider-specific agent implementation stays behind the AI provider boundary.
 */

#include "umicom/helix/agent.h"
#include <stddef.h>

#include <stdio.h>
#include <string.h>

/*
 * Initialise helix agent from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_helix_agent_init(UmiHelixAgent *agent,
                               const char *agent_id,
                               UmiHelixAgentKind kind,
                               const char *provider_id,
                               const char *permission)
{
    int a;
    int b;
    int c;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL || agent_id == NULL || provider_id == NULL || permission == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(agent, 0, sizeof(*agent));
    a = snprintf(agent->agent_id, sizeof(agent->agent_id), "%s", agent_id);
    b = snprintf(agent->provider_id, sizeof(agent->provider_id), "%s", provider_id);
    c = snprintf(agent->permission, sizeof(agent->permission), "%s", permission);
    /* Apply this branch only when its contract condition is satisfied. */
    if (a < 0 || b < 0 || c < 0 ||
        (size_t)a >= sizeof(agent->agent_id) ||
        (size_t)b >= sizeof(agent->provider_id) ||
        (size_t)c >= sizeof(agent->permission)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    agent->kind = kind;
    agent->enabled = 1;
    return UMI_STATUS_OK;
}
