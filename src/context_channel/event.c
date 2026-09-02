/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/event.c
 *
 * PURPOSE:
 *   Implement publish typed context lifecycle events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/event.h"
#include <string.h>
/*
 * Initialise context event state from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_event_state_init(UmiContextEventState *state)
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
 * Provide the context event state record operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_event_state_record(UmiContextEventState *state,UmiStatus status,const char *message)
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
 * Return the number of records represented by context event operation without changing
 * their state.
 */
uint64_t umi_context_event_operation_count(const UmiContextEventState *state){
return state==NULL?0U:state->operation_count;
}
