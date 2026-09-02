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
/*
 * Initialise panel context state from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_context_state_init(UmiPanelContextState *state,const char *id){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(state==NULL)return;
memset(state,0,sizeof(*state));
state->following=true;
state->revision=1U;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(id!=NULL)(void)umi_context_copy_text(state->panel_instance_id,sizeof(state->panel_instance_id),id);
}
/*
 * Provide the panel context state bind operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_context_state_bind(UmiPanelContextState *state,const char *channel_id){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(state==NULL||channel_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
{
UmiStatus s=umi_context_copy_text(state->channel_id,sizeof(state->channel_id),channel_id);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(s==UMI_STATUS_OK)state->revision+=1U;
return s;
}
}
/*
 * Perform panel context state through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_panel_context_state_apply(UmiPanelContextState *state,const UmiContextPayload *payload){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(state==NULL||payload==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_context_payload_validate(payload)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
state->current=*payload;
state->has_context=true;
state->revision+=1U;
return UMI_STATUS_OK;
}
/*
 * Release or reset state held by panel context state so the same storage can be reused
 * safely.
 */
void umi_panel_context_state_clear(UmiPanelContextState *state){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(state==NULL)return;
memset(&state->current,0,sizeof(state->current));
state->has_context=false;
state->revision+=1U;
}
/*
 * Provide the panel context state accepts operation used by this module and its client
 * applications.
 */
bool umi_panel_context_state_accepts(const UmiPanelContextState *state,const char *channel_id){
return state!=NULL&&channel_id!=NULL&&state->following&&strcmp(state->channel_id,channel_id)==0;
}
