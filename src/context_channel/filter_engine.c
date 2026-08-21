/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/filter_engine.c
 *
 * PURPOSE:
 *   Implement apply declared field filters before routing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/filter_engine.h"
#include <string.h>
void umi_context_filter_engine_state_init(UmiContextFilterEngineState *state)
{
if(state==NULL)return;
memset(state,0,sizeof(*state));
state->revision=1U;
state->last_status=UMI_STATUS_OK;
}
UmiStatus umi_context_filter_engine_state_record(UmiContextFilterEngineState *state,UmiStatus status,const char *message)
{
if(state==NULL||message==NULL)return UMI_STATUS_INVALID_ARGUMENT;
state->operation_count+=1U;
state->revision+=1U;
state->last_status=status;
(void)umi_context_copy_text(state->last_message,sizeof(state->last_message),message);
return UMI_STATUS_OK;
}
uint64_t umi_context_filter_engine_operation_count(const UmiContextFilterEngineState *state){
return state==NULL?0U:state->operation_count;
}
