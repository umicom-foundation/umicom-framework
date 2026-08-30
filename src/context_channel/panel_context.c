/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/panel_context.c
 *
 * PURPOSE:
 *   Maintain current context state for one cross-application panel instance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/panel_context.h"
#include <string.h>
void umi_panel_context_state_init(UmiPanelContextState *state,const char *id){
if(state==NULL)return;
memset(state,0,sizeof(*state));
state->following=true;
state->revision=1U;
if(id!=NULL)(void)umi_context_copy_text(state->panel_instance_id,sizeof(state->panel_instance_id),id);
}
UmiStatus umi_panel_context_state_bind(UmiPanelContextState *state,const char *channel_id){
if(state==NULL||channel_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
{
UmiStatus s=umi_context_copy_text(state->channel_id,sizeof(state->channel_id),channel_id);
if(s==UMI_STATUS_OK)state->revision+=1U;
return s;
}
}
UmiStatus umi_panel_context_state_apply(UmiPanelContextState *state,const UmiContextPayload *payload){
if(state==NULL||payload==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(umi_context_payload_validate(payload)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
state->current=*payload;
state->has_context=true;
state->revision+=1U;
return UMI_STATUS_OK;
}
void umi_panel_context_state_clear(UmiPanelContextState *state){
if(state==NULL)return;
memset(&state->current,0,sizeof(state->current));
state->has_context=false;
state->revision+=1U;
}
bool umi_panel_context_state_accepts(const UmiPanelContextState *state,const char *channel_id){
return state!=NULL&&channel_id!=NULL&&state->following&&strcmp(state->channel_id,channel_id)==0;
}
