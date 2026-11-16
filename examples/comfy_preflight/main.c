/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_preflight/main.c
 *
 * PURPOSE:
 *   Demonstrate native workflow diagnostics with synthetic ComfyUI metadata and no provider execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_preflight.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    UmiProviderConnection connection={0};
    strcpy(connection.id,"comfy-example");strcpy(connection.provider_id,"comfyui");
    strcpy(connection.label,"Synthetic preflight example");
    strcpy(connection.endpoint,"http://127.0.0.1:8188");
    connection.enabled=true;connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.timeout_ms=30000;
    /* These bytes are a teaching fixture, not a discovered server capability. */
    const char catalogue_json[]="{\"ExampleNode\":{\"input\":{\"required\":{\"text\":[\"STRING\"]}},\"output\":[]}}";
    const char workflow_json[]="{\"1\":{\"class_type\":\"ExampleNode\",\"inputs\":{}}}";
    UmiComfyPromptDraft draft={0};draft.workflow_json=workflow_json;draft.workflow_length=sizeof workflow_json-1U;
    strcpy(draft.client_id,"sdk-preflight-example");
    UmiComfyPrompt *prompt=NULL;UmiComfyNodeCatalogue *catalogue=NULL;UmiComfyPreflightReport *report=NULL;
    UmiStatus status=UmiComfyPromptCreate(&connection,1U,&draft,NULL,&prompt);
    if(status==UMI_STATUS_OK)status=UmiComfyNodeCatalogueCreate(&connection,1U,catalogue_json,sizeof catalogue_json-1U,NULL,&catalogue);
    if(status==UMI_STATUS_OK)status=UmiComfyPreflightCreate(&connection,1U,prompt,catalogue,NULL,&report);
    UmiComfyPreflightSummary summary={0};UmiComfyPreflightIssue issue={0};
    if(status==UMI_STATUS_OK)status=UmiComfyPreflightInspect(report,&summary);
    if(status==UMI_STATUS_OK)status=UmiComfyPreflightIssueAt(report,0U,&issue);
    int result=1;
    if(status==UMI_STATUS_OK && summary.missing_required_count==1U && !summary.all_node_requirements_observed &&
       issue.kind==UMI_COMFY_PREFLIGHT_INPUT_MISSING && strcmp(issue.input_name,"text")==0){
        printf("Node %s (%s) needs input %s. No request was submitted.\n",issue.node_id,issue.class_type,issue.input_name);
        result=0;
    }else fprintf(stderr,"Synthetic preflight did not produce its expected missing-input report (status %d).\n",(int)status);
    UmiComfyPreflightDestroy(report);UmiComfyNodeCatalogueDestroy(catalogue);UmiComfyPromptDestroy(prompt);
    return result;
}
