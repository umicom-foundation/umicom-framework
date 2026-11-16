/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workflow/test_allocation.c
 *
 * PURPOSE:
 *   Verify workflow workspace ownership and complete output preservation across allocation and planner refusals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
static bool fail_workspace;
static bool fail_plan;
static size_t live_workspaces;
static size_t workspace_attempts;

static void *workflow_test_calloc(size_t count, size_t size)
{
    ++workspace_attempts;
    if(fail_workspace) return NULL;
    void *memory=calloc(count,size);
    if(memory!=NULL) ++live_workspaces;
    return memory;
}
static void workflow_test_free(void *memory)
{
    if(memory!=NULL) --live_workspaces;
    free(memory);
}
static UmiStatus workflow_test_plan(const UmiFabricWorkflowGraph *graph,
    UmiFabricWorkflowPlan *plan)
{
    if(fail_plan) return UMI_STATUS_OUT_OF_MEMORY;
    return umi_fabric_workflow_plan_build(graph,plan);
}
/* Compile the actual producer with private allocator boundaries. The shared
 * planner's own allocation behavior is covered by its separate native tests. */
UmiStatus umi_test_ai_workflow_preflight(const UmiAiWorkflowDescription *description,
    uint32_t allowed_effects, UmiAiWorkflowPlan *out_plan,
    UmiAiWorkflowDiagnostic *diagnostic);
#define calloc workflow_test_calloc
#define free workflow_test_free
#define umi_fabric_workflow_plan_build workflow_test_plan
#define umi_ai_workflow_preflight umi_test_ai_workflow_preflight
#include "../../src/ai_workflow/preflight.c"
#undef umi_ai_workflow_preflight
#undef umi_fabric_workflow_plan_build
#undef free
#undef calloc

#define CHECK(condition) do { if (!(condition)) {     fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while (0)

static int exercise(WorkflowFixture *f, const char *mode)
{
    if(strcmp(mode,"workspace")==0) fail_workspace=true;
    else if(strcmp(mode,"planner")==0) fail_plan=true;
    else if(strcmp(mode,"cleanup")==0) f->links[0].source_node=SIZE_MAX;
    else return 2;
    UmiStatus status=umi_test_ai_workflow_preflight(&f->description,0U,&f->plan,&f->diagnostic);
    CHECK(status==(strcmp(mode,"cleanup")==0?UMI_STATUS_INVALID_ARGUMENT:UMI_STATUS_OUT_OF_MEMORY));
    CHECK(f->diagnostic.issue==(strcmp(mode,"cleanup")==0?
        UMI_AI_WORKFLOW_ISSUE_LINK_NODE:UMI_AI_WORKFLOW_ISSUE_ALLOCATION));
    CHECK(live_workspaces==0U && workspace_attempts==1U);
    CHECK(memcmp(&f->plan,f->before,sizeof(f->plan))==0);
    fail_workspace=false;fail_plan=false;f->links[0].source_node=0U;
    CHECK(umi_test_ai_workflow_preflight(&f->description,0U,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
    CHECK(live_workspaces==0U && workspace_attempts==2U);
    CHECK(f->plan.execution_ready && f->plan.order.count==3U);
    CHECK(umi_test_ai_workflow_preflight(&f->description,0U,NULL,&f->diagnostic)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(workspace_attempts==2U);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    WorkflowFixture *fixture=workflow_fixture_create();
    if(fixture==NULL) return 2;
    int result=exercise(fixture,argv[1]);
    free(fixture);
    return result;
}
