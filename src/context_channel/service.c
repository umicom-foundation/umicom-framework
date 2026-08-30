/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/service.c
 *
 * PURPOSE:
 *   Implement coordinate registry, routing, delivery, history, audit and metrics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/service.h"
#include <string.h>
void umi_context_service_state_init(UmiContextServiceState *state)
{
if(state==NULL)return;
memset(state,0,sizeof(*state));
state->revision=1U;
state->last_status=UMI_STATUS_OK;
}
UmiStatus umi_context_service_state_record(UmiContextServiceState *state,UmiStatus status,const char *message)
{
if(state==NULL||message==NULL)return UMI_STATUS_INVALID_ARGUMENT;
state->operation_count+=1U;
state->revision+=1U;
state->last_status=status;
(void)umi_context_copy_text(state->last_message,sizeof(state->last_message),message);
return UMI_STATUS_OK;
}
uint64_t umi_context_service_operation_count(const UmiContextServiceState *state){
return state==NULL?0U:state->operation_count;
}
