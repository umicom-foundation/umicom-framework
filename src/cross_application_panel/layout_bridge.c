/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/layout_bridge.c
 *
 * PURPOSE:
 *   Implement reusable panel layout bridge service evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/layout_bridge.h"
#include <string.h>
/*
 * Initialise panel layout bridge state from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_layout_bridge_state_init(UmiPanelLayoutBridgeState *state){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(state==NULL)return;
memset(state,0,sizeof(*state));
state->revision=1U;
state->last_status=UMI_STATUS_OK;
}
/*
 * Provide the panel layout bridge record operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_layout_bridge_record(UmiPanelLayoutBridgeState *state,UmiStatus status,const char *message){
UmiStatus copy;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(state==NULL||message==NULL)return UMI_STATUS_INVALID_ARGUMENT;
copy=umi_context_copy_text(state->last_message,sizeof(state->last_message),message);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(copy!=UMI_STATUS_OK)return copy;
state->last_status=status;
state->operation_count+=1U;
state->revision+=1U;
return UMI_STATUS_OK;
}
/*
 * Provide the panel layout bridge healthy operation used by this module and its client
 * applications.
 */
bool umi_panel_layout_bridge_healthy(const UmiPanelLayoutBridgeState *state){
return state!=NULL&&state->last_status==UMI_STATUS_OK;
}
/*
 * Return the number of records represented by panel layout bridge operation without
 * changing their state.
 */
uint64_t umi_panel_layout_bridge_operation_count(const UmiPanelLayoutBridgeState *state){
return state==NULL?0U:state->operation_count;
}
