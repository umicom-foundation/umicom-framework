/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_references.c
 *
 * PURPOSE:
 *   Reuse prepared workflow and catalogue owners to report reference-shaped input values with explicit unknown metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_references.h"
#include "comfy_response_internal.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>
struct UmiComfyReferenceInventory {
    UmiComfyReferenceSummary summary;
    UmiComfyReference *items;
};
static bool Candidate(const UmiJsonTree *tree, int value)
{
    return UmiJsonTreeKind(tree,value)==UMI_LANGUAGE_RUNTIME_JSON_ARRAY &&
        UmiJsonTreeCount(tree,value)==2U &&
        UmiJsonTreeKind(tree,UmiJsonTreeFirst(tree,value))==UMI_LANGUAGE_RUNTIME_JSON_STRING;
}
/* Admission is a separate pass: allocate exactly the bounded item count and
 * reject unrepresentable names before publishing any copied observation. */
static UmiStatus Count(const UmiJsonTree *tree, const UmiCancellationToken *cancel,
    size_t *out_count, size_t *out_other)
{
    size_t count=0U,other=0U;
    for(int key=UmiJsonTreeFirst(tree,0);key>=0;){
        int node=UmiJsonTreeNext(tree,key),inputs=-1;
        UmiStatus status=UmiComfyResponseMember(tree,node,"inputs",&inputs);
        if(status!=UMI_STATUS_OK)return status;
        for(int input=UmiJsonTreeFirst(tree,inputs);input>=0;){
            if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
            int value=UmiJsonTreeNext(tree,input);
            if(Candidate(tree,value)){
                if(count==UMI_COMFY_REFERENCE_LIMIT)return UMI_STATUS_CAPACITY_EXCEEDED;
                char text[129];
                status=UmiJsonTreeText(tree,input,text,sizeof text);
                if(status==UMI_STATUS_OK)status=UmiJsonTreeText(tree,UmiJsonTreeFirst(tree,value),text,sizeof text);
                if(status!=UMI_STATUS_OK)return status;
                ++count;
            }else if(UmiJsonTreeKind(tree,value)==UMI_LANGUAGE_RUNTIME_JSON_ARRAY)++other;
            input=UmiJsonTreeNext(tree,value);
        }
        key=UmiJsonTreeNext(tree,node);
    }
    *out_count=count;*out_other=other;return UMI_STATUS_OK;
}
static UmiStatus Observe(UmiComfyReferenceInventory *inventory, size_t index,
    const UmiJsonTree *tree, int key, int input, int value,
    const UmiComfyNodeCatalogue *catalogue)
{
    UmiComfyReference *item=&inventory->items[index];
    int first=UmiJsonTreeFirst(tree,value),slot=UmiJsonTreeNext(tree,first);
    UmiStatus status=UmiJsonTreeText(tree,key,item->node_id,sizeof item->node_id);
    if(status==UMI_STATUS_OK)status=UmiJsonTreeText(tree,input,item->input_name,sizeof item->input_name);
    if(status==UMI_STATUS_OK)status=UmiJsonTreeText(tree,first,item->target_id,sizeof item->target_id);
    if(status!=UMI_STATUS_OK)return status;
    const char *spelling=NULL;size_t length=0U;
    status=UmiJsonTreeSourceSpan(tree,slot,&spelling,&length);
    if(status!=UMI_STATUS_OK)return status;
    /* The shared exact integer reader rejects fractional/overflow values.
     * Checking the first byte separately also rejects negative-zero spelling. */
    item->slot_known=length>0U && spelling[0]>='0' && spelling[0]<='9' &&
        UmiJsonTreeInteger(tree,slot,&item->output_slot)==UMI_STATUS_OK;
    if(!item->slot_known)++inventory->summary.unsupported_slot_count;
    int target=-1;
    status=UmiJsonTreeMember(tree,0,item->target_id,&target);
    if(status==UMI_STATUS_NOT_FOUND){++inventory->summary.missing_target_count;return UMI_STATUS_OK;}
    if(status!=UMI_STATUS_OK)return status;
    item->target_found=true;
    status=UmiComfyResponseText(tree,target,"class_type",item->target_class,sizeof item->target_class,0);
    if(status!=UMI_STATUS_OK)return status;
    size_t class_index=0U;
    status=UmiComfyNodeCatalogueFind(catalogue,item->target_class,&class_index);
    if(status==UMI_STATUS_NOT_FOUND){++inventory->summary.unobserved_output_count;return UMI_STATUS_OK;}
    if(status!=UMI_STATUS_OK)return status;
    UmiComfyNodeInfo info;
    status=UmiComfyNodeCatalogueAt(catalogue,class_index,&info);
    if(status!=UMI_STATUS_OK)return status;
    item->output_count_known=true;item->declared_output_count=info.output_count;
    if(!item->slot_known)return UMI_STATUS_OK;
    /* Compare before narrowing to size_t; the observed output limit is small
     * even on a host whose size_t cannot hold every signed 64-bit slot. */
    if((uint64_t)item->output_slot>=(uint64_t)info.output_count){
        ++inventory->summary.out_of_range_count;return UMI_STATUS_OK;
    }
    status=UmiComfyNodeCatalogueOutputAt(catalogue,class_index,(size_t)item->output_slot,
        item->output_type,sizeof item->output_type);
    if(status==UMI_STATUS_OK){item->output_observed=true;++inventory->summary.observed_output_count;}
    return status;
}
void UmiComfyReferenceInventoryDestroy(UmiComfyReferenceInventory *inventory)
{
    if(!inventory)return;
    if(inventory->items){
        umi_secret_clear(inventory->items,inventory->summary.candidate_count*sizeof *inventory->items);
        free(inventory->items);
    }
    umi_secret_clear(inventory,sizeof *inventory);free(inventory);
}
UmiStatus UmiComfyReferenceInventoryCreate(const UmiProviderConnection *current,
    uint64_t current_revision,const UmiComfyPrompt *prompt,
    const UmiComfyNodeCatalogue *catalogue,const UmiCancellationToken *cancel,
    UmiComfyReferenceInventory **out)
{
    if(!out || *out || !current || !prompt || !catalogue)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
    UmiStatus status=UmiComfyPromptCheckConnection(prompt,current,current_revision);
    if(status==UMI_STATUS_OK)status=UmiComfyNodeCatalogueCheckConnection(catalogue,current,current_revision);
    if(status!=UMI_STATUS_OK)return status;
    const char *workflow=NULL,*body=NULL;size_t bytes=0U;
    UmiComfyPromptReview prompt_review;UmiComfyNodeCatalogueReview catalogue_review;
    status=UmiComfyPromptInspect(prompt,&prompt_review,&body);
    if(status==UMI_STATUS_OK)status=UmiComfyNodeCatalogueInspect(catalogue,&catalogue_review);
    if(status==UMI_STATUS_OK)status=UmiComfyPromptWorkflow(prompt,&workflow,&bytes);
    UmiJsonTree *tree=NULL;
    if(status==UMI_STATUS_OK)status=UmiComfyResponseOpen(workflow,bytes,UMI_COMFY_PROMPT_MAX_BYTES,cancel,&tree);
    if(status!=UMI_STATUS_OK)return status;
    size_t count=0U,other=0U;
    status=Count(tree,cancel,&count,&other);
    UmiComfyReferenceInventory *candidate=NULL;
    if(status==UMI_STATUS_OK){candidate=calloc(1U,sizeof *candidate);if(!candidate)status=UMI_STATUS_OUT_OF_MEMORY;}
    if(status==UMI_STATUS_OK){
        candidate->summary.candidate_count=count;candidate->summary.other_array_count=other;
        candidate->summary.node_count=prompt_review.node_count;
        candidate->summary.connection_revision=prompt_review.connection_revision;
        memcpy(candidate->summary.connection_sha256,prompt_review.connection_sha256,UMI_SHA256_BYTES);
        memcpy(candidate->summary.request_sha256,prompt_review.request_sha256,UMI_SHA256_BYTES);
        memcpy(candidate->summary.catalogue_sha256,catalogue_review.response_sha256,UMI_SHA256_BYTES);
        if(count){candidate->items=calloc(count,sizeof *candidate->items);if(!candidate->items)status=UMI_STATUS_OUT_OF_MEMORY;}
    }
    size_t next=0U;
    for(int key=UmiJsonTreeFirst(tree,0);key>=0 && status==UMI_STATUS_OK;){
        int node=UmiJsonTreeNext(tree,key),inputs=-1;
        status=UmiComfyResponseMember(tree,node,"inputs",&inputs);
        for(int input=UmiJsonTreeFirst(tree,inputs);input>=0 && status==UMI_STATUS_OK;){
            if(umi_cancellation_token_is_requested(cancel)){status=UMI_STATUS_CANCELLED;break;}
            int value=UmiJsonTreeNext(tree,input);
            if(Candidate(tree,value))status=Observe(candidate,next++,tree,key,input,value,catalogue);
            input=UmiJsonTreeNext(tree,value);
        }
        key=UmiJsonTreeNext(tree,node);
    }
    UmiJsonTreeDestroy(tree);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK)*out=candidate;else UmiComfyReferenceInventoryDestroy(candidate);
    return status;
}
UmiStatus UmiComfyReferenceInventoryInspect(const UmiComfyReferenceInventory *inventory,UmiComfyReferenceSummary *out)
{
    if(!inventory || !out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=inventory->summary;return UMI_STATUS_OK;
}
UmiStatus UmiComfyReferenceInventoryAt(const UmiComfyReferenceInventory *inventory,size_t index,UmiComfyReference *out)
{
    if(!inventory || !out)return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=inventory->summary.candidate_count)return UMI_STATUS_NOT_FOUND;
    *out=inventory->items[index];return UMI_STATUS_OK;
}
