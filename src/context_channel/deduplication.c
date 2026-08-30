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
void umi_context_deduplication_init(UmiContextDeduplication *state){
if(state!=NULL)memset(state,0,sizeof(*state));
}
bool umi_context_deduplication_seen(const UmiContextDeduplication *state,const UmiContextPayload *payload){
size_t i;
if(state==NULL||payload==NULL)return false;
for(i=0U;i<state->count;++i)if(strcmp(state->entries[i].context_id,payload->identity.context_id)==0&&state->entries[i].content_hash==payload->content_hash&&state->entries[i].sequence==payload->identity.sequence)return true;
return false;
}
UmiStatus umi_context_deduplication_record(UmiContextDeduplication *state,const UmiContextPayload *payload){
UmiContextDeduplicationEntry *e;
if(state==NULL||payload==NULL)return UMI_STATUS_INVALID_ARGUMENT;
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
if(state->count<UMI_CONTEXT_MAX_RECENT_IDS)state->count+=1U;
return UMI_STATUS_OK;
}
