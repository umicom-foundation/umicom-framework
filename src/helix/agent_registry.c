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

/*
 * Initialise helix agent registry from caller-provided values so later operations receive
 * a known state.
 */
void umi_helix_agent_registry_init(UmiHelixAgentRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}

/* Add helix agent registry only after its inputs and available capacity have been checked. */
UmiStatus umi_helix_agent_registry_add(UmiHelixAgentRegistry *registry,
                                       const UmiHelixAgent *agent)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || agent == NULL || agent->agent_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_helix_agent_registry_find(registry, agent->agent_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_HELIX_MAX_AGENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->agents[registry->count++] = *agent;
    return UMI_STATUS_OK;
}

/*
 * Find helix agent registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiHelixAgent *umi_helix_agent_registry_find(UmiHelixAgentRegistry *registry,
                                             const char *agent_id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || agent_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->agents[i].agent_id, agent_id) == 0) return &registry->agents[i];
    }
    return NULL;
}
