/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/helix_service.h
 *
 * PURPOSE:
 *   Provide the top-level Framework-owned Helix control-plane state and guarded session transitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_HELIX_SERVICE_H
#define UMICOM_DEVELOPER_HELIX_HELIX_SERVICE_H
#include "umicom/developer/helix/helix_session.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixService { UmiHelixSession sessions[UMI_HELIX_MAX_ITEMS]; size_t count; UmiHelixAutonomyMode autonomy_ceiling; bool repository_mutation_enabled; } UmiHelixService;
/* Initialise the Framework-owned Helix control plane with supervised defaults. */
void umi_helix_service_init(UmiHelixService *service);
/* Create a bounded session under the service autonomy ceiling. */
UmiStatus umi_helix_service_open(UmiHelixService *service,const char *id,uint32_t max_iterations,UmiHelixSession **out_session);
/* Explicitly enable or disable repository mutation for the control plane. */
void umi_helix_service_set_mutation(UmiHelixService *service,bool enabled);
/* Decide whether a session may enter running mutation state. */
UmiHelixDecision umi_helix_service_execution_decision(const UmiHelixService *service,const UmiHelixSession *session);
#ifdef __cplusplus
}
#endif
#endif
