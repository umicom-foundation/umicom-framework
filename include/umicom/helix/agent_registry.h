/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/agent_registry.h
 *
 * PURPOSE:
 *   Register and resolve bounded Helix specialist agents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The registry provides deterministic agent discovery and lets Studio show exactly which specialist agents are enabled.
 */

#ifndef INCLUDE_UMICOM_HELIX_AGENT_REGISTRY_H
#define INCLUDE_UMICOM_HELIX_AGENT_REGISTRY_H

#include <stddef.h>
#include "umicom/helix/agent.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the helix agent registry data shared with callers of this public contract.
 */
typedef struct UmiHelixAgentRegistry {
    UmiHelixAgent agents[UMI_HELIX_MAX_AGENTS];
    size_t count;
} UmiHelixAgentRegistry;

/**
 * Initialise helix agent registry from caller-provided values so later operations receive
 * a known state.
 */
void umi_helix_agent_registry_init(UmiHelixAgentRegistry *registry);
/**
 * Add helix agent registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_helix_agent_registry_add(UmiHelixAgentRegistry *registry,
                                       const UmiHelixAgent *agent);
/**
 * Find helix agent registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiHelixAgent *umi_helix_agent_registry_find(UmiHelixAgentRegistry *registry,
                                             const char *agent_id);

#ifdef __cplusplus
}
#endif

#endif
