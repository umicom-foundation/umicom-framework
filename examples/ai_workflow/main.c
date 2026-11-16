/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/ai_workflow/main.c
 *
 * PURPOSE:
 *   Demonstrate typed image-workflow planning with an unavailable provider from an installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_workflow/preflight.h"
#include <stdio.h>

int main(void)
{
    /* These describe a proposed pipeline. Availability is a host observation,
     * and this small program deliberately has no inference executor. */
    UmiAiWorkflowNodeType types[] = {
        {.operation_id="prompt.source",
         .outputs={{"text",UMI_AI_WORKFLOW_TEXT,false}},.output_count=1U,.available=true},
        {.operation_id="image.inference",
         .inputs={{"prompt",UMI_AI_WORKFLOW_TEXT,true}},.input_count=1U,
         .outputs={{"image",UMI_AI_WORKFLOW_IMAGE,false}},.output_count=1U,.available=false},
        {.operation_id="brand.watermark",
         .inputs={{"image",UMI_AI_WORKFLOW_IMAGE,true}},.input_count=1U,
         .outputs={{"branded_image",UMI_AI_WORKFLOW_IMAGE,false}},.output_count=1U,.available=false},
        {.operation_id="image.export",
         .inputs={{"image",UMI_AI_WORKFLOW_IMAGE,true}},.input_count=1U,
         .available=true,.effects=UMI_AI_WORKFLOW_EFFECT_WRITE_FILE}
    };
    UmiAiWorkflowNode nodes[] = {
        {"prompt",0U,0U},{"generate",1U,0U},{"watermark",2U,0U},{"export",3U,0U}
    };
    UmiAiWorkflowLink links[] = {{0U,0U,1U,0U},{1U,0U,2U,0U},{2U,0U,3U,0U}};
    size_t outputs[] = {3U};
    UmiAiWorkflowDescription description = {types,4U,nodes,4U,links,3U,outputs,1U};
    UmiAiWorkflowPlan plan = {0};
    UmiAiWorkflowDiagnostic diagnostic = {0};
    /* Allowing a declared effect in this plan does not grant a destination
     * path or cause a file write. A real export needs separate scoped approval. */
    UmiStatus status = umi_ai_workflow_preflight(&description,
        UMI_AI_WORKFLOW_EFFECT_WRITE_FILE,&plan,&diagnostic);
    if(status!=UMI_STATUS_OK) {
        fprintf(stderr,"Umicom workflow: %s\n",umi_ai_workflow_issue_text(diagnostic.issue));
        return 1;
    }
    puts("Umicom Framework: typed workflow plan");
    for(size_t i=0U;i<plan.order.count;++i)
        printf("%zu. %s\n",i+1U,plan.order.step_ids[i]);
    printf("Host-declared ready: %s\n",plan.execution_ready?"yes":"no");
    for(size_t i=0U;i<plan.unavailable_count;++i) {
        size_t index=plan.unavailable_indices[i];
        printf("Unavailable: %s\n",types[nodes[index].type_index].operation_id);
    }
    puts("Planning only. No provider, model, watermark or export operation ran.");
    return 0;
}
