/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_references/main.c
 *
 * PURPOSE:
 *   Review synthetic ComfyUI output references using the installed native SDK without contacting a server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_references.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    const char workflow[]="{\"source\":{\"class_type\":\"Image\",\"inputs\":{}},"
        "\"sink\":{\"class_type\":\"Save\",\"inputs\":{\"image\":[\"source\",0],\"extra\":[\"missing\",0]}}}";
    const char nodes[]="{\"Image\":{\"input\":{},\"output\":[\"IMAGE\"]},\"Save\":{\"input\":{},\"output\":[]}}";
    UmiProviderConnection connection={0};
    strcpy(connection.id,"comfy-example");strcpy(connection.provider_id,"comfyui");
    strcpy(connection.label,"Synthetic observation");strcpy(connection.endpoint,"http://127.0.0.1:8188");
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.enabled=true;connection.timeout_ms=30000;
    UmiComfyPromptDraft draft={0};draft.workflow_json=workflow;draft.workflow_length=sizeof workflow-1U;
    strcpy(draft.client_id,"reference-example");
    UmiComfyPrompt *prompt=NULL;UmiComfyNodeCatalogue *catalogue=NULL;UmiComfyReferenceInventory *inventory=NULL;
    UmiStatus status=UmiComfyPromptCreate(&connection,1U,&draft,NULL,&prompt);
    if(status==UMI_STATUS_OK)status=UmiComfyNodeCatalogueCreate(&connection,1U,nodes,sizeof nodes-1U,NULL,&catalogue);
    if(status==UMI_STATUS_OK)status=UmiComfyReferenceInventoryCreate(&connection,1U,prompt,catalogue,NULL,&inventory);
    /* The inventory keeps only copied identities and observations. It remains
     * readable after both input owners are destroyed; no prompt values leak. */
    UmiComfyPromptDestroy(prompt);UmiComfyNodeCatalogueDestroy(catalogue);
    UmiComfyReferenceSummary summary;
    if(status==UMI_STATUS_OK)status=UmiComfyReferenceInventoryInspect(inventory,&summary);
    int failed=status!=UMI_STATUS_OK;
    if(!failed){
        failed=summary.candidate_count!=2U || summary.observed_output_count!=1U || summary.missing_target_count!=1U;
        if(!failed && printf("Candidates: %zu; observed outputs: %zu; missing targets: %zu\n",
                summary.candidate_count,summary.observed_output_count,summary.missing_target_count)<0)failed=1;
    }
    UmiComfyReferenceInventoryDestroy(inventory);
    if(fflush(stdout)!=0)failed=1;
    return failed?1:0;
}
