/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workflow/test_memory_allocation.c
 *
 * PURPOSE:
 *   Inject failures at actual workflow planning allocation boundaries and verify every owned workspace is released.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include "umicom/ai_workflow/memory_plan.h"
static size_t allocation_attempts,live_allocations;
static bool fail_allocation,fail_preflight;
static void *memory_test_calloc(size_t count,size_t size)
{
    ++allocation_attempts;if(fail_allocation)return NULL;
    void *result=calloc(count,size);if(result!=NULL)++live_allocations;return result;
}
static void memory_test_free(void *memory)
{
    if(memory!=NULL)--live_allocations;free(memory);
}
static UmiStatus memory_test_preflight(const UmiAiWorkflowDescription *description,
    uint32_t effects,UmiAiWorkflowPlan *plan,UmiAiWorkflowDiagnostic *diagnostic)
{
    if(fail_preflight){diagnostic->issue=UMI_AI_WORKFLOW_ISSUE_ALLOCATION;return UMI_STATUS_OUT_OF_MEMORY;}
    return umi_ai_workflow_preflight(description,effects,plan,diagnostic);
}
UmiStatus umi_test_workflow_memory_plan(const UmiAiWorkflowDescription *,
    const UmiAiWorkflowNodeMemory *,size_t,const UmiAiWorkflowMemoryPolicy *,uint32_t,
    UmiAiWorkflowMemoryPlan *,UmiAiWorkflowMemoryDiagnostic *);
/* Exercise the actual C producer under private allocation boundaries; no fake
 * arithmetic implementation substitutes for production failure cleanup. */
#define calloc memory_test_calloc
#define free memory_test_free
#define umi_ai_workflow_preflight memory_test_preflight
#define umi_ai_workflow_memory_plan umi_test_workflow_memory_plan
#include "../../src/ai_workflow/memory_plan.c"
#undef umi_ai_workflow_memory_plan
#undef umi_ai_workflow_preflight
#undef free
#undef calloc
#define CHECK(c) do {if(!(c)){fprintf(stderr,"line %d: %s\n",__LINE__,#c);return 1;}}while(0)
static int exercise(WorkflowFixture *f,UmiAiWorkflowMemoryPlan *plan,const char *mode)
{
    UmiAiWorkflowNodeMemory costs[3]={
        {.output_bytes={2U},.known_outputs=1U,.scratch_known=true},
        {.output_bytes={3U},.known_outputs=1U,.scratch_known=true},
        {.scratch_known=true}};
    UmiAiWorkflowMemoryPolicy policy={.byte_limit=10U,.resident_known=true};
    UmiAiWorkflowMemoryDiagnostic diagnostic={0};
    memset(plan,0xa5,sizeof(*plan));
    UmiStatus expected=UMI_STATUS_OK;
    if(strcmp(mode,"workspace")==0){fail_allocation=true;expected=UMI_STATUS_OUT_OF_MEMORY;}
    else if(strcmp(mode,"preflight")==0){fail_preflight=true;expected=UMI_STATUS_OUT_OF_MEMORY;}
    else if(strcmp(mode,"cost")==0){costs[0].known_outputs=0U;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"overflow")==0){policy.resident_bytes=UINT64_MAX;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(strcmp(mode,"arguments")==0)expected=UMI_STATUS_INVALID_ARGUMENT;
    else if(strcmp(mode,"success")!=0 && strcmp(mode,"repeated")!=0)return 2;
    size_t repetitions=strcmp(mode,"repeated")==0?200U:1U;
    for(size_t n=0U;n<repetitions;++n) {
        CHECK(umi_test_workflow_memory_plan(strcmp(mode,"arguments")==0?NULL:&f->description,
            costs,3U,&policy,0U,plan,&diagnostic)==expected);
        CHECK(live_allocations==0U);
    }
    CHECK(allocation_attempts==(strcmp(mode,"arguments")==0?0U:repetitions));
    if(expected!=UMI_STATUS_OK) {
        const unsigned char *bytes=(const unsigned char *)plan;
        for(size_t i=0U;i<sizeof(*plan);++i)CHECK(bytes[i]==0xa5U);
    } else CHECK(plan->peak_bytes==5U && plan->final_live_bytes==0U);
    fail_allocation=false;fail_preflight=false;costs[0].known_outputs=1U;policy.resident_bytes=0U;
    CHECK(umi_test_workflow_memory_plan(&f->description,costs,3U,&policy,0U,plan,&diagnostic)==UMI_STATUS_OK);
    CHECK(live_allocations==0U && plan->peak_bytes==5U);return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    WorkflowFixture *f=workflow_fixture_create();UmiAiWorkflowMemoryPlan *plan=malloc(sizeof(*plan));
    if(f==NULL || plan==NULL){free(plan);free(f);return 2;}
    int result=exercise(f,plan,argv[1]);free(plan);free(f);return result;
}
