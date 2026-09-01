/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/agent_registry.c
 *
 * PURPOSE:
 *   Register and resolve Helix specialist agents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Duplicate IDs are rejected so orchestration cannot accidentally route work to an ambiguous agent.
 */

#include "umicom/helix/agent_registry.h"
#include <stddef.h>

#include <string.h>

void umi_helix_agent_registry_init(UmiHelixAgentRegistry *registry)
{
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}

UmiStatus umi_helix_agent_registry_add(UmiHelixAgentRegistry *registry,
                                       const UmiHelixAgent *agent)
{
    if (registry == NULL || agent == NULL || agent->agent_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_helix_agent_registry_find(registry, agent->agent_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count >= UMI_HELIX_MAX_AGENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->agents[registry->count++] = *agent;
    return UMI_STATUS_OK;
}

UmiHelixAgent *umi_helix_agent_registry_find(UmiHelixAgentRegistry *registry,
                                             const char *agent_id)
{
    size_t i;
    if (registry == NULL || agent_id == NULL) return NULL;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->agents[i].agent_id, agent_id) == 0) return &registry->agents[i];
    }
    return NULL;
}
