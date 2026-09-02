/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/agent.h
 *
 * PURPOSE:
 *   Define one bounded Helix specialist agent and its declared permissions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Specialist agents declare one role and permission boundary; the Helix coordinator does not give a model unrestricted access to every operation.
 */

#ifndef INCLUDE_UMICOM_HELIX_AGENT_H
#define INCLUDE_UMICOM_HELIX_AGENT_H

#include <stdint.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the helix agent data shared with callers of this public contract.
 */
typedef struct UmiHelixAgent {
    char agent_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixAgentKind kind;
    char provider_id[UMI_HELIX_ID_CAPACITY];
    char permission[UMI_HELIX_ID_CAPACITY];
    uint32_t priority;
    int enabled;
} UmiHelixAgent;

/**
 * Initialise helix agent from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_helix_agent_init(UmiHelixAgent *agent,
                               const char *agent_id,
                               UmiHelixAgentKind kind,
                               const char *provider_id,
                               const char *permission);

#ifdef __cplusplus
}
#endif

#endif
