/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/agent.h
 *
 * PURPOSE:
 *   Define one bounded Helix specialist agent and its declared permissions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiHelixAgent {
    char agent_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixAgentKind kind;
    char provider_id[UMI_HELIX_ID_CAPACITY];
    char permission[UMI_HELIX_ID_CAPACITY];
    uint32_t priority;
    int enabled;
} UmiHelixAgent;

UmiStatus umi_helix_agent_init(UmiHelixAgent *agent,
                               const char *agent_id,
                               UmiHelixAgentKind kind,
                               const char *provider_id,
                               const char *permission);

#ifdef __cplusplus
}
#endif

#endif
