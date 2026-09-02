/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/snapshot.c
 *
 * PURPOSE:
 *   Implement expose immutable runtime snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/snapshot.h"
#include <string.h>
/*
 * Initialise context snapshot state from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_snapshot_state_init(UmiContextSnapshotState *state)
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
 * Provide the context snapshot state record operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_snapshot_state_record(UmiContextSnapshotState *state,UmiStatus status,const char *message)
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
 * Return the number of records represented by context snapshot operation without changing
 * their state.
 */
uint64_t umi_context_snapshot_operation_count(const UmiContextSnapshotState *state){
return state==NULL?0U:state->operation_count;
}
