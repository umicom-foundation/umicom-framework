/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_queue.c
 *
 * PURPOSE:
 *   Admit complete queue documents through shared JSON ownership and retain only copied scheduling metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_queue.h"
#include "comfy_response_internal.h"
#include "umicom/base/utf8.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>
struct UmiComfyQueue {
    UmiComfyQueueReview review;
    size_t count;
    UmiComfyQueueEntry *entries;
    UmiComfyQueueEntry **by_id;
};
static int CompareIds(const void *left,const void *right)
{
    const UmiComfyQueueEntry *const *a=left,*const *b=right;
    return strcmp((*a)->prompt_id,(*b)->prompt_id);
}
static UmiStatus ReadEntry(const UmiJsonTree *tree,int node,UmiComfyQueueState state,
    UmiComfyQueueEntry *out)
{
    if(UmiJsonTreeKind(tree,node)!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY || UmiJsonTreeCount(tree,node)<5U)
        return UMI_STATUS_PARSE_ERROR;
    int number=UmiJsonTreeFirst(tree,node),id=UmiJsonTreeNext(tree,number);
    int prompt=UmiJsonTreeNext(tree,id),extra=UmiJsonTreeNext(tree,prompt),outputs=UmiJsonTreeNext(tree,extra);
    if(UmiJsonTreeKind(tree,number)!=UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE ||
       UmiJsonTreeKind(tree,id)!=UMI_LANGUAGE_RUNTIME_JSON_STRING ||
       UmiJsonTreeKind(tree,prompt)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
       UmiJsonTreeKind(tree,extra)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
       UmiJsonTreeKind(tree,outputs)!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY)return UMI_STATUS_PARSE_ERROR;
    const char *spelling=NULL;size_t length=0U;
    UmiStatus status=UmiJsonTreeSourceSpan(tree,number,&spelling,&length);
    if(status!=UMI_STATUS_OK)return status;
    /* Shared JSON admission proved number grammar; distinguish it from the
     * other primitive kinds without losing exact spelling through a double. */
    if(length==0U || !((spelling[0]>='0' && spelling[0]<='9') || spelling[0]=='-'))return UMI_STATUS_PARSE_ERROR;
    if(length>=sizeof out->number_json)return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out->number_json,spelling,length);out->number_json[length]='\0';out->state=state;
    status=UmiJsonTreeText(tree,id,out->prompt_id,sizeof out->prompt_id);
    if(status!=UMI_STATUS_OK)return status;
    if(out->prompt_id[0]=='\0')return UMI_STATUS_PARSE_ERROR;
    for(int value=UmiJsonTreeFirst(tree,outputs);value>=0;value=UmiJsonTreeNext(tree,value)){
        if(UmiJsonTreeKind(tree,value)!=UMI_LANGUAGE_RUNTIME_JSON_STRING)return UMI_STATUS_PARSE_ERROR;
        status=UmiJsonTreeSourceSpan(tree,value,&spelling,&length);
        if(status!=UMI_STATUS_OK)return status;
        if(length<=2U)return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
void UmiComfyQueueDestroy(UmiComfyQueue *queue)
{
    if(queue==NULL)return;
    if(queue->entries){umi_secret_clear(queue->entries,queue->count*sizeof *queue->entries);free(queue->entries);}
    if(queue->by_id){umi_secret_clear(queue->by_id,queue->count*sizeof *queue->by_id);free(queue->by_id);}
    umi_secret_clear(queue,sizeof *queue);free(queue);
}
UmiStatus UmiComfyQueueCreate(const UmiProviderConnection *connection,uint64_t revision,
    const void *bytes,size_t length,const UmiCancellationToken *cancel,UmiComfyQueue **out)
{
    if(out==NULL || *out!=NULL || revision==0U)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
    UmiStatus status=UmiComfyConnectionCheck(connection);
    if(status!=UMI_STATUS_OK)return status;
    UmiJsonTree *tree=NULL;status=UmiComfyResponseOpen(bytes,length,UMI_COMFY_QUEUE_MAX_BYTES,cancel,&tree);
    if(status!=UMI_STATUS_OK)return status;
    int lists[2]={-1,-1};size_t counts[2]={0U,0U};
    status=UmiComfyResponseMember(tree,0,"queue_running",&lists[0]);
    if(status==UMI_STATUS_OK)status=UmiComfyResponseMember(tree,0,"queue_pending",&lists[1]);
    for(size_t i=0U;i<2U && status==UMI_STATUS_OK;++i){
        if(UmiJsonTreeKind(tree,lists[i])!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY)status=UMI_STATUS_PARSE_ERROR;
        else counts[i]=UmiJsonTreeCount(tree,lists[i]);
    }
    if(status==UMI_STATUS_OK && (counts[0]>UMI_COMFY_QUEUE_LIMIT || counts[1]>UMI_COMFY_QUEUE_LIMIT-counts[0]))
        status=UMI_STATUS_CAPACITY_EXCEEDED;
    UmiComfyQueue *candidate=NULL;
    if(status==UMI_STATUS_OK){candidate=calloc(1U,sizeof *candidate);if(!candidate)status=UMI_STATUS_OUT_OF_MEMORY;}
    if(status==UMI_STATUS_OK){
        candidate->count=counts[0]+counts[1];candidate->review.running_count=counts[0];
        candidate->review.pending_count=counts[1];candidate->review.connection_revision=revision;
        candidate->review.response_bytes=length;
        if(candidate->count){
            candidate->entries=calloc(candidate->count,sizeof *candidate->entries);
            candidate->by_id=calloc(candidate->count,sizeof *candidate->by_id);
            if(!candidate->entries || !candidate->by_id)status=UMI_STATUS_OUT_OF_MEMORY;
        }
    }
    size_t index=0U;
    for(size_t group=0U;group<2U && status==UMI_STATUS_OK;++group){
        for(int node=UmiJsonTreeFirst(tree,lists[group]);node>=0 && status==UMI_STATUS_OK;node=UmiJsonTreeNext(tree,node)){
            if(umi_cancellation_token_is_requested(cancel)){status=UMI_STATUS_CANCELLED;break;}
            status=ReadEntry(tree,node,group==0U?UMI_COMFY_QUEUE_RUNNING:UMI_COMFY_QUEUE_PENDING,&candidate->entries[index]);
            if(status==UMI_STATUS_OK){candidate->by_id[index]=&candidate->entries[index];++index;}
        }
    }
    if(status==UMI_STATUS_OK && candidate->count>1U){
        /* A separate index preserves wire order and makes duplicate identity
         * checks independent of which list contains the conflicting entry. */
        qsort(candidate->by_id,candidate->count,sizeof *candidate->by_id,CompareIds);
        for(size_t i=1U;i<candidate->count;++i){
            if(umi_cancellation_token_is_requested(cancel)){status=UMI_STATUS_CANCELLED;break;}
            if(strcmp(candidate->by_id[i-1U]->prompt_id,candidate->by_id[i]->prompt_id)==0){status=UMI_STATUS_ALREADY_EXISTS;break;}
        }
    }
    if(status==UMI_STATUS_OK)status=UmiProviderConnectionFingerprint(connection,candidate->review.connection_sha256);
    if(status==UMI_STATUS_OK){
        UmiSha256 digest;UmiSha256Init(&digest);status=UmiSha256Update(&digest,bytes,length);
        if(status==UMI_STATUS_OK)status=UmiSha256Final(&digest,candidate->review.response_sha256);
        umi_secret_clear(&digest,sizeof digest);
    }
    /* Prompt and extra-data bytes are temporary parser ownership only. */
    UmiJsonTreeDestroy(tree);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK)*out=candidate;else UmiComfyQueueDestroy(candidate);
    return status;
}
size_t UmiComfyQueueCount(const UmiComfyQueue *queue){return queue?queue->count:0U;}
UmiStatus UmiComfyQueueInspect(const UmiComfyQueue *queue,UmiComfyQueueReview *out)
{
    if(!queue || !out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=queue->review;return UMI_STATUS_OK;
}
UmiStatus UmiComfyQueueAt(const UmiComfyQueue *queue,size_t index,UmiComfyQueueEntry *out)
{
    if(!queue || !out)return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=queue->count)return UMI_STATUS_NOT_FOUND;
    *out=queue->entries[index];return UMI_STATUS_OK;
}
UmiStatus UmiComfyQueueFind(const UmiComfyQueue *queue,const char *id,size_t *out_index)
{
    if(!queue || !id || !out_index)return UMI_STATUS_INVALID_ARGUMENT;
    size_t length=0U;while(length<UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY && id[length]!='\0')++length;
    if(length==0U)return UMI_STATUS_INVALID_ARGUMENT;
    if(length==UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;
    if(!UmiUtf8Validate((const unsigned char *)id,length,NULL))return UMI_STATUS_INVALID_ARGUMENT;
    size_t low=0U,high=queue->count;
    while(low<high){size_t middle=low+(high-low)/2U;int order=strcmp(id,queue->by_id[middle]->prompt_id);
        if(order==0){*out_index=(size_t)(queue->by_id[middle]-queue->entries);return UMI_STATUS_OK;}
        if(order<0)high=middle;else low=middle+1U;
    }
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus UmiComfyQueueCheckConnection(const UmiComfyQueue *queue,const UmiProviderConnection *current,uint64_t revision)
{
    if(!queue || !current)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=UmiComfyConnectionCheck(current);if(status!=UMI_STATUS_OK)return status;
    if(revision!=queue->review.connection_revision)return UMI_STATUS_BUSY;
    unsigned char digest[UMI_SHA256_BYTES];status=UmiProviderConnectionFingerprint(current,digest);
    if(status==UMI_STATUS_OK && memcmp(digest,queue->review.connection_sha256,sizeof digest)!=0)status=UMI_STATUS_BUSY;
    return status;
}
