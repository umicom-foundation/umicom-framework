/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/group_service.c
 *
 * PURPOSE:
 *   Implement manage typed colour groups and membership.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/group_service.h"
#include <string.h>
void umi_context_group_service_state_init(UmiContextGroupServiceState *state)
{
if(state==NULL)return;
memset(state,0,sizeof(*state));
state->revision=1U;
state->last_status=UMI_STATUS_OK;
}
UmiStatus umi_context_group_service_state_record(UmiContextGroupServiceState *state,UmiStatus status,const char *message)
{
if(state==NULL||message==NULL)return UMI_STATUS_INVALID_ARGUMENT;
state->operation_count+=1U;
state->revision+=1U;
state->last_status=status;
(void)umi_context_copy_text(state->last_message,sizeof(state->last_message),message);
return UMI_STATUS_OK;
}
uint64_t umi_context_group_service_operation_count(const UmiContextGroupServiceState *state){
return state==NULL?0U:state->operation_count;
}
