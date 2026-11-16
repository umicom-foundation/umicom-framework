/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/ai_workflow_memory/main.c
 *
 * PURPOSE:
 *   Demonstrate installed-SDK workflow memory planning with copied observations and no model loading.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_workflow/memory_plan.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv)
{
    bool incomplete=argc==2 && strcmp(argv[1],"--unknown-scratch")==0;
    if(argc!=1 && !incomplete){fprintf(stderr,"Usage: umicom-workflow-memory [--unknown-scratch]\n");return 2;}
    const UmiAiWorkflowNodeType types[]={
        {.operation_id="image.source",.outputs={{"image",UMI_AI_WORKFLOW_IMAGE,false}},.output_count=1U,.available=true},
        {.operation_id="image.edit",.inputs={{"image",UMI_AI_WORKFLOW_IMAGE,true}},.input_count=1U,
         .outputs={{"image",UMI_AI_WORKFLOW_IMAGE,false}},.output_count=1U,.available=true},
        {.operation_id="image.inspect",.inputs={{"image",UMI_AI_WORKFLOW_IMAGE,true}},.input_count=1U,.available=true}};
    const UmiAiWorkflowNode nodes[]={{"source",0U,0U},{"edit",1U,0U},{"inspect",2U,0U}};
    const UmiAiWorkflowLink links[]={{0U,0U,1U,0U},{1U,0U,2U,0U}};
    const size_t outputs[]={2U};
    const UmiAiWorkflowDescription description={types,3U,nodes,3U,links,2U,outputs,1U};
    const UmiAiWorkflowNodeMemory costs[]={
        {.output_bytes={100U},.known_outputs=1U,.scratch_bytes=5U,.scratch_known=true},
        {.output_bytes={40U},.known_outputs=1U,.scratch_bytes=incomplete?0U:20U,.scratch_known=!incomplete},
        {.scratch_bytes=7U,.scratch_known=true}};
    const UmiAiWorkflowMemoryPolicy policy={.resident_bytes=10U,.byte_limit=200U,.resident_known=true};
    UmiAiWorkflowMemoryPlan *plan=calloc(1U,sizeof(*plan));
    if(plan==NULL)return 1;
    UmiAiWorkflowMemoryDiagnostic diagnostic={0};
    UmiStatus status=umi_ai_workflow_memory_plan(&description,costs,3U,&policy,0U,plan,&diagnostic);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Planning refused: issue=%d\n",(int)diagnostic.issue);free(plan);return 1;}
    printf("Umicom workflow memory: declared=%s peak=%" PRIu64 " final=%" PRIu64 "\n",
        plan->estimates_complete?"complete":"incomplete",plan->peak_bytes,plan->final_live_bytes);
    for(size_t i=0U;i<plan->workflow.order.count;++i)
        printf("%s: before=%" PRIu64 " peak=%" PRIu64 " release=%" PRIu64 " after=%" PRIu64 "\n",
            plan->workflow.order.step_ids[i],plan->steps[i].live_before_bytes,
            plan->steps[i].peak_bytes,plan->steps[i].released_bytes,plan->steps[i].live_after_bytes);
    int result=plan->peak_bytes==(incomplete?150U:170U) && plan->final_live_bytes==10U?0:1;
    free(plan);if(fflush(stdout)!=0 || ferror(stdout))return 1;return result;
}
