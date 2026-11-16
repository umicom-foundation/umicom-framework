/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/retrieval_sources/main.c
 *
 * PURPOSE:
 *   Build a source-backed local draft with the installed SDK using an inspected immutable search result and explicit review.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_workspace/search_snapshot.h"
#include "umicom/ai_workspace/evidence.h"
#include "umicom/ai_workspace/providers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    UmiDataServer *data=NULL;UmiAiRuntime runtime;UmiAiWorkspace *workspace=NULL;
    UmiAiSearchSnapshot *search=NULL;UmiAiWorkspaceJob *job=calloc(1U,sizeof *job);
    UmiAiSearchSelection *selection=calloc(1U,sizeof *selection);
    umi_ai_runtime_init(&runtime);
    UmiStatus status=(job==NULL || selection==NULL)?UMI_STATUS_OUT_OF_MEMORY:UMI_STATUS_OK;
    UmiAiProvider provider;
    if(status==UMI_STATUS_OK)status=umi_data_server_create_memory(&data);
    if(status==UMI_STATUS_OK)status=UmiAiWorkspaceExtractiveProviderCreate(&provider);
    if(status==UMI_STATUS_OK)status=umi_ai_provider_registry_add(&runtime.providers,&provider);
    if(status==UMI_STATUS_OK)status=UmiAiWorkspaceCreate(data,&runtime,"sdk.sources",&workspace);
    if(status==UMI_STATUS_OK)status=UmiAiWorkspacePutCollection(workspace,"workshop","Workshop notices");
    if(status==UMI_STATUS_OK)status=UmiAiWorkspacePutSource(workspace,"opening","workshop","Opening notice",
        "The workshop opens at 10:00 on Saturday.",7U);
    if(status==UMI_STATUS_OK)status=UmiAiWorkspacePutSource(workspace,"parking","workshop","Parking notice",
        "The workshop parking entrance is at the west gate.",12U);
    if(status==UMI_STATUS_OK)status=UmiAiWorkspaceSearchCapture(workspace,"workshop","workshop",
        NULL,NULL,NULL,4U,&search);
    UmiAiSearchSummary summary;
    if(status==UMI_STATUS_OK)status=UmiAiSearchSnapshotInspect(search,&summary);
    if(status==UMI_STATUS_OK && summary.count!=2U)status=UMI_STATUS_INVALID_STATE;
    if(status==UMI_STATUS_OK){
        printf("Captured %zu ranked sources at corpus revision %llu.\n",summary.count,(unsigned long long)summary.corpusRevision);
        for(size_t i=0U;i<summary.count && status==UMI_STATUS_OK;++i){
            UmiAiWorkspaceEvidence row;status=UmiAiSearchSnapshotAt(search,i,&row);
            if(status==UMI_STATUS_OK)printf("%zu: %s, lines %u-%u\n",i,row.source.title,row.source.firstLine,row.source.lastLine);
        }
    }
    /* A real UI obtains these indices from explicit user selection. This offline
     * lesson chooses a deliberate order so that preparation can be inspected. */
    const size_t selected[]={1U,0U};
    if(status==UMI_STATUS_OK)status=UmiAiSearchSnapshotSelect(search,selected,2U,selection);
    if(status==UMI_STATUS_OK)status=UmiAiWorkspacePrepareEvidence(workspace,"draft",
        "umicom.extractive-preview","extractive-preview",selection->collection.id,
        "Summarise the workshop notices.","learner",256U,selection->corpusRevision,
        selection->evidence,selection->count);
    if(status==UMI_STATUS_OK)status=UmiAiWorkspaceJobFind(workspace,"draft",job);
    if(status==UMI_STATUS_OK && (job->state!=UMI_AI_WORKSPACE_REVIEW ||
        strcmp(job->evidence[0].source.id,"parking")))status=UMI_STATUS_INVALID_STATE;
    if(status==UMI_STATUS_OK)status=UmiAiWorkspaceReview(workspace,"draft","lesson-reviewer",true);
    if(status==UMI_STATUS_OK)status=UmiAiWorkspaceRun(workspace,"draft",NULL);
    if(status==UMI_STATUS_OK)status=UmiAiWorkspaceJobFind(workspace,"draft",job);
    if(status==UMI_STATUS_OK && job->state!=UMI_AI_WORKSPACE_SUCCEEDED)status=UMI_STATUS_INVALID_STATE;
    if(status==UMI_STATUS_OK)printf("Extractive preview, not model inference:\n%s\n",job->response.text);
    if(status==UMI_STATUS_OK)status=UmiAiWorkspacePutSource(workspace,"parking","workshop","Parking notice",
        "The workshop parking entrance is now at the east gate.",12U);
    if(status==UMI_STATUS_OK){
        UmiStatus stale=UmiAiWorkspacePrepareEvidence(workspace,"stale-draft",
            "umicom.extractive-preview","extractive-preview",selection->collection.id,
            "Summarise the workshop notices.","learner",256U,selection->corpusRevision,
            selection->evidence,selection->count);
        if(stale!=UMI_STATUS_BUSY)status=UMI_STATUS_INVALID_STATE;
    }
    /* The search owner survives workspace destruction. It intentionally retains
     * the old west-gate passage; it is not a refresh of the current collection. */
    UmiAiWorkspaceDestroy(workspace);workspace=NULL;
    umi_data_server_destroy(data);data=NULL;umi_ai_runtime_destroy(&runtime);
    if(status==UMI_STATUS_OK){
        UmiAiWorkspaceEvidence row;status=UmiAiSearchSnapshotAt(search,1U,&row);
        if(status==UMI_STATUS_OK && strstr(row.source.text,"west gate")==NULL)status=UMI_STATUS_INVALID_STATE;
    }
    UmiAiSearchSnapshotDestroy(search);free(selection);free(job);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Source lesson failed with status %d.\n",(int)status);return 1;}
    puts("Stale selection refused; the historical snapshot remained readable. No network or filesystem operation was requested.");
    return fflush(stdout)==0 && !ferror(stdout)?0:1;
}
