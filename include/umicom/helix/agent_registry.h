/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/agent_registry.h
 *
 * PURPOSE:
 *   Register and resolve bounded Helix specialist agents.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The registry provides deterministic agent discovery and lets Studio show exactly which specialist agents are enabled.
 */

#ifndef INCLUDE_UMICOM_HELIX_AGENT_REGISTRY_H
#define INCLUDE_UMICOM_HELIX_AGENT_REGISTRY_H

#include <stddef.h>
#include "umicom/helix/agent.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixAgentRegistry {
    UmiHelixAgent agents[UMI_HELIX_MAX_AGENTS];
    size_t count;
} UmiHelixAgentRegistry;

void umi_helix_agent_registry_init(UmiHelixAgentRegistry *registry);
UmiStatus umi_helix_agent_registry_add(UmiHelixAgentRegistry *registry,
                                       const UmiHelixAgent *agent);
UmiHelixAgent *umi_helix_agent_registry_find(UmiHelixAgentRegistry *registry,
                                             const char *agent_id);

#ifdef __cplusplus
}
#endif

#endif
