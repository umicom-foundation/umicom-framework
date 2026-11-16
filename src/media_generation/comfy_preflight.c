/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_preflight.c
 *
 * PURPOSE:
 *   Compose prepared workflows and captured node schemas into bounded, immutable native diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_preflight.h"
#include "comfy_response_internal.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>
struct UmiComfyPreflightReport {
    UmiComfyPreflightSummary summary;
    UmiComfyPreflightIssue issues[UMI_COMFY_PREFLIGHT_STORED_ISSUES];
};
static void Record(UmiComfyPreflightReport *report,UmiComfyPreflightIssueKind kind,
    const char *id,const char *type,const char *input)
{
    /* Counts remain complete after the display budget is exhausted. Maximum
     * counts are bounded by 1024 workflow nodes and 256 required fields each. */
    ++report->summary.issue_count;
    if(report->summary.stored_issue_count==UMI_COMFY_PREFLIGHT_STORED_ISSUES){
        ++report->summary.omitted_issue_count;return;
    }
    UmiComfyPreflightIssue *issue=&report->issues[report->summary.stored_issue_count++];
    issue->kind=kind;strcpy(issue->node_id,id);strcpy(issue->class_type,type);
    if(input)strcpy(issue->input_name,input);
}
static UmiStatus InspectNode(UmiComfyPreflightReport *report,const UmiJsonTree *tree,
    int key,const UmiComfyNodeCatalogue *catalogue,const UmiCancellationToken *cancel)
{
    char id[129],type[129];int node=UmiJsonTreeNext(tree,key),inputs=-1;
    UmiStatus status=UmiJsonTreeText(tree,key,id,sizeof id);
    if(status==UMI_STATUS_OK)status=UmiComfyResponseText(tree,node,"class_type",type,sizeof type,0);
    if(status==UMI_STATUS_OK)status=UmiComfyResponseMember(tree,node,"inputs",&inputs);
    if(status!=UMI_STATUS_OK)return status;
    size_t class_index=0U;
    status=UmiComfyNodeCatalogueFind(catalogue,type,&class_index);
    if(status==UMI_STATUS_NOT_FOUND){
        ++report->summary.unknown_node_count;
        Record(report,UMI_COMFY_PREFLIGHT_CLASS_MISSING,id,type,NULL);return UMI_STATUS_OK;
    }
    if(status!=UMI_STATUS_OK)return status;
    ++report->summary.known_node_count;
    UmiComfyNodeInfo info;
    status=UmiComfyNodeCatalogueAt(catalogue,class_index,&info);
    if(status!=UMI_STATUS_OK)return status;
    for(size_t i=0U;i<info.required_count;++i){
        if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
        UmiComfyNodeInput input;int value=-1;
        status=UmiComfyNodeCatalogueInputAt(catalogue,class_index,UMI_COMFY_INPUT_REQUIRED,i,&input);
        if(status!=UMI_STATUS_OK)return status;
        status=UmiJsonTreeMember(tree,inputs,input.name,&value);
        if(status==UMI_STATUS_NOT_FOUND){
            ++report->summary.missing_required_count;
            Record(report,UMI_COMFY_PREFLIGHT_INPUT_MISSING,id,type,input.name);
        }else if(status!=UMI_STATUS_OK)return status;
    }
    if(!(info.known_flags&UMI_COMFY_NODE_OUTPUT_NODE))++report->summary.unknown_output_flag_count;
    else if(info.enabled_flags&UMI_COMFY_NODE_OUTPUT_NODE)++report->summary.observed_output_count;
    if(info.enabled_flags&UMI_COMFY_NODE_DEPRECATED){
        ++report->summary.deprecated_count;Record(report,UMI_COMFY_PREFLIGHT_DEPRECATED,id,type,NULL);
    }
    if(info.enabled_flags&UMI_COMFY_NODE_EXPERIMENTAL){
        ++report->summary.experimental_count;Record(report,UMI_COMFY_PREFLIGHT_EXPERIMENTAL,id,type,NULL);
    }
    if(info.enabled_flags&UMI_COMFY_NODE_API_NODE){
        ++report->summary.api_node_count;Record(report,UMI_COMFY_PREFLIGHT_API_NODE,id,type,NULL);
    }
    return UMI_STATUS_OK;
}
void UmiComfyPreflightDestroy(UmiComfyPreflightReport *report)
{
    if(!report)return;
    umi_secret_clear(report,sizeof *report);free(report);
}
UmiStatus UmiComfyPreflightCreate(const UmiProviderConnection *current,
    uint64_t current_revision,const UmiComfyPrompt *prompt,const UmiComfyNodeCatalogue *catalogue,
    const UmiCancellationToken *cancel,UmiComfyPreflightReport **out)
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
    UmiComfyPreflightReport *candidate=calloc(1U,sizeof *candidate);
    if(!candidate){UmiJsonTreeDestroy(tree);return UMI_STATUS_OUT_OF_MEMORY;}
    candidate->summary.node_count=prompt_review.node_count;
    candidate->summary.connection_revision=prompt_review.connection_revision;
    memcpy(candidate->summary.connection_sha256,prompt_review.connection_sha256,UMI_SHA256_BYTES);
    memcpy(candidate->summary.request_sha256,prompt_review.request_sha256,UMI_SHA256_BYTES);
    memcpy(candidate->summary.catalogue_sha256,catalogue_review.response_sha256,UMI_SHA256_BYTES);
    for(int key=UmiJsonTreeFirst(tree,0);key>=0 && status==UMI_STATUS_OK;){
        if(umi_cancellation_token_is_requested(cancel)){status=UMI_STATUS_CANCELLED;break;}
        status=InspectNode(candidate,tree,key,catalogue,cancel);
        key=UmiJsonTreeNext(tree,UmiJsonTreeNext(tree,key));
    }
    UmiJsonTreeDestroy(tree);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK){
        candidate->summary.all_node_requirements_observed=
            candidate->summary.unknown_node_count==0U && candidate->summary.missing_required_count==0U;
        *out=candidate;
    }else UmiComfyPreflightDestroy(candidate);
    return status;
}
UmiStatus UmiComfyPreflightInspect(const UmiComfyPreflightReport *report,UmiComfyPreflightSummary *out)
{
    if(!report || !out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=report->summary;return UMI_STATUS_OK;
}
UmiStatus UmiComfyPreflightIssueAt(const UmiComfyPreflightReport *report,size_t index,UmiComfyPreflightIssue *out)
{
    if(!report || !out)return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=report->summary.stored_issue_count)return UMI_STATUS_NOT_FOUND;
    *out=report->issues[index];return UMI_STATUS_OK;
}
