/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/bridge.c
 *
 * PURPOSE:
 *   Implement provide compact product-facing publishers for canonical context types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/bridge.h"
#include <string.h>
/*
 * Initialise context bridge state from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_bridge_state_init(UmiContextBridgeState *state)
{
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
 * Provide the context bridge state record operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_bridge_state_record(UmiContextBridgeState *state,UmiStatus status,const char *message)
{
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(state==NULL||message==NULL)return UMI_STATUS_INVALID_ARGUMENT;
state->operation_count+=1U;
state->revision+=1U;
state->last_status=status;
(void)umi_context_copy_text(state->last_message,sizeof(state->last_message),message);
return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by context bridge operation without changing
 * their state.
 */
uint64_t umi_context_bridge_operation_count(const UmiContextBridgeState *state){
return state==NULL?0U:state->operation_count;
}
