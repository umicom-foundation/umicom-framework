/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/deduplication.c
 *
 * PURPOSE:
 *   Prevent context feedback loops and duplicate propagation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/deduplication.h"
#include <string.h>
/*
 * Initialise context deduplication from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_deduplication_init(UmiContextDeduplication *state){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(state!=NULL)memset(state,0,sizeof(*state));
}
/*
 * Provide the context deduplication seen operation used by this module and its client
 * applications.
 */
bool umi_context_deduplication_seen(const UmiContextDeduplication *state,const UmiContextPayload *payload){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(state==NULL||payload==NULL)return false;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<state->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(state->entries[i].context_id,payload->identity.context_id)==0&&state->entries[i].content_hash==payload->content_hash&&state->entries[i].sequence==payload->identity.sequence)return true;
return false;
}
/*
 * Provide the context deduplication record operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_deduplication_record(UmiContextDeduplication *state,const UmiContextPayload *payload){
UmiContextDeduplicationEntry *e;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(state==NULL||payload==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Apply this branch only when its contract condition is satisfied. */
if(umi_context_deduplication_seen(state,payload)){
state->duplicate_count+=1U;
return UMI_STATUS_ALREADY_EXISTS;
}
e=&state->entries[state->next_index];
memset(e,0,sizeof(*e));
(void)umi_context_copy_text(e->context_id,sizeof(e->context_id),payload->identity.context_id);
e->content_hash=payload->content_hash;
e->sequence=payload->identity.sequence;
state->next_index=(state->next_index+1U)%UMI_CONTEXT_MAX_RECENT_IDS;
/* Keep the operation inside its valid bounds before reading, writing or adding data. */
if(state->count<UMI_CONTEXT_MAX_RECENT_IDS)state->count+=1U;
return UMI_STATUS_OK;
}
