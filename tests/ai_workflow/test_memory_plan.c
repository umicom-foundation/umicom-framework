/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workflow/test_memory_plan.c
 *
 * PURPOSE:
 *   Check known-byte peaks, shared-value last use, incomplete costs, overflow refusals and copied planning ownership.
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
#define CHECK(c) do {if(!(c)){fprintf(stderr,"line %d: %s\n",__LINE__,#c);return 1;}}while(0)
typedef struct MemoryFixture {
    UmiAiWorkflowNodeMemory costs[UMI_AI_WORKFLOW_MAX_NODES];
    UmiAiWorkflowMemoryPolicy policy;
    UmiAiWorkflowMemoryPlan plan;
    unsigned char before[sizeof(UmiAiWorkflowMemoryPlan)];
    UmiAiWorkflowMemoryDiagnostic diagnostic;
} MemoryFixture;
static int exercise(WorkflowFixture *f,MemoryFixture *m,const char *mode)
{
    UmiAiWorkflowDescription *d=&f->description;
    m->costs[0]=(UmiAiWorkflowNodeMemory){.output_bytes={100U},.known_outputs=1U,.scratch_bytes=5U,.scratch_known=true};
    m->costs[1]=(UmiAiWorkflowNodeMemory){.output_bytes={40U},.known_outputs=1U,.scratch_bytes=20U,.scratch_known=true};
    m->costs[2]=(UmiAiWorkflowNodeMemory){.scratch_bytes=7U,.scratch_known=true};
    m->policy=(UmiAiWorkflowMemoryPolicy){.resident_bytes=10U,.byte_limit=200U,.resident_known=true};
    memset(&m->plan,0xa5,sizeof(m->plan));memcpy(m->before,&m->plan,sizeof(m->plan));
    size_t count=d->node_count;uint32_t effects=0U;
    UmiStatus expected=UMI_STATUS_OK;
    UmiAiWorkflowMemoryIssue issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_NONE;
    bool recognized=true;
    if(strcmp(mode,"chain")==0 || strcmp(mode,"copied-lifetime")==0 || strcmp(mode,"repeated")==0) {}
    else if(strcmp(mode,"exact-limit")==0) m->policy.byte_limit=170U;
    else if(strcmp(mode,"over-limit")==0) m->policy.byte_limit=150U;
    else if(strcmp(mode,"zero-cost")==0 || strcmp(mode,"zero-limit")==0 || strcmp(mode,"resident-only")==0) {
        for(size_t n=0U;n<3U;++n){m->costs[n].output_bytes[0]=0U;m->costs[n].scratch_bytes=0U;}
        if(strcmp(mode,"resident-only")!=0) m->policy.resident_bytes=0U;
        if(strcmp(mode,"zero-limit")==0) m->policy.byte_limit=0U;
    }
    else if(strcmp(mode,"retained-result")==0) f->outputs[0]=1U;
    else if(strcmp(mode,"selected-ancestor")==0){f->outputs[1]=0U;d->output_count=2U;}
    else if(strcmp(mode,"fanout")==0) {
        f->nodes[3]=(UmiAiWorkflowNode){"n-right",1U,0U};d->node_count=4U;count=4U;
        f->types[2].inputs[1]=(UmiAiWorkflowPort){"other",UMI_AI_WORKFLOW_TEXT,true};f->types[2].input_count=2U;
        f->links[2]=(UmiAiWorkflowLink){0U,0U,3U,0U};f->links[3]=(UmiAiWorkflowLink){3U,0U,2U,1U};d->link_count=4U;
        m->costs[3]=(UmiAiWorkflowNodeMemory){.output_bytes={60U},.known_outputs=1U,.scratch_bytes=20U,.scratch_known=true};
    }
    else if(strcmp(mode,"multiple-input-links")==0) {
        f->types[1].inputs[1]=(UmiAiWorkflowPort){"other",UMI_AI_WORKFLOW_TEXT,true};f->types[1].input_count=2U;
        f->links[2]=(UmiAiWorkflowLink){0U,0U,1U,1U};d->link_count=3U;
    }
    else if(strcmp(mode,"dead-port")==0) {
        f->types[0].outputs[1]=(UmiAiWorkflowPort){"unused",UMI_AI_WORKFLOW_TEXT,false};f->types[0].output_count=2U;
        m->costs[0].output_bytes[1]=200U;m->costs[0].known_outputs=3U;
    }
    else if(strcmp(mode,"inactive-unknown")==0) {f->nodes[3]=(UmiAiWorkflowNode){"inactive",0U,0U};d->node_count=4U;count=4U;}
    else if(strcmp(mode,"selected-unknown-output")==0){m->costs[0].known_outputs=0U;m->costs[0].output_bytes[0]=0U;}
    else if(strcmp(mode,"unknown-scratch")==0){m->costs[1].scratch_known=false;m->costs[1].scratch_bytes=0U;}
    else if(strcmp(mode,"unknown-resident")==0){m->policy.resident_known=false;m->policy.resident_bytes=0U;}
    else if(strcmp(mode,"unknown-with-bytes")==0){m->costs[0].known_outputs=0U;expected=UMI_STATUS_INVALID_ARGUMENT;issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_COST;}
    else if(strcmp(mode,"unknown-scratch-with-bytes")==0){m->costs[1].scratch_known=false;expected=UMI_STATUS_INVALID_ARGUMENT;issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_COST;}
    else if(strcmp(mode,"unknown-resident-with-bytes")==0){m->policy.resident_known=false;expected=UMI_STATUS_INVALID_ARGUMENT;issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_COST;}
    else if(strcmp(mode,"unused-cost-slot")==0){m->costs[0].output_bytes[7]=1U;expected=UMI_STATUS_INVALID_ARGUMENT;issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_COST;}
    else if(strcmp(mode,"invalid-mask")==0){m->costs[0].known_outputs=UINT32_MAX;expected=UMI_STATUS_INVALID_ARGUMENT;issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_COST;}
    else if(strcmp(mode,"cost-count")==0){count=SIZE_MAX;expected=UMI_STATUS_INVALID_ARGUMENT;issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_ARGUMENT;}
    else if(strncmp(mode,"null-",5U)==0){expected=UMI_STATUS_INVALID_ARGUMENT;issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_ARGUMENT;}
    else if(strcmp(mode,"output-sum-overflow")==0) {
        f->types[0].outputs[1]=(UmiAiWorkflowPort){"extra",UMI_AI_WORKFLOW_TEXT,false};f->types[0].output_count=2U;
        m->costs[0].known_outputs=3U;m->costs[0].output_bytes[0]=UINT64_MAX;m->costs[0].output_bytes[1]=1U;
        expected=UMI_STATUS_CAPACITY_EXCEEDED;issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_OVERFLOW;
    }
    else if(strcmp(mode,"live-overflow")==0){m->policy.resident_bytes=UINT64_MAX;expected=UMI_STATUS_CAPACITY_EXCEEDED;issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_OVERFLOW;}
    else if(strcmp(mode,"scratch-overflow")==0){m->costs[1].scratch_bytes=UINT64_MAX;expected=UMI_STATUS_CAPACITY_EXCEEDED;issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_OVERFLOW;}
    else if(strcmp(mode,"maximum-integer")==0) {
        d->node_count=1U;count=1U;d->link_count=0U;f->outputs[0]=0U;
        m->costs[0].output_bytes[0]=UINT64_MAX;m->costs[0].scratch_bytes=0U;m->policy.resident_bytes=0U;m->policy.byte_limit=UINT64_MAX;
    }
    else if(strcmp(mode,"invalid-graph")==0){f->links[0].source_node=SIZE_MAX;expected=UMI_STATUS_INVALID_ARGUMENT;issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_WORKFLOW;}
    else if(strcmp(mode,"denied-effect")==0 || strcmp(mode,"allowed-effect")==0) {
        f->types[2].effects=UMI_AI_WORKFLOW_EFFECT_WRITE_FILE;
        if(strcmp(mode,"allowed-effect")==0)effects=UMI_AI_WORKFLOW_EFFECT_WRITE_FILE;
        else {expected=UMI_STATUS_PERMISSION_DENIED;issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_WORKFLOW;}
    }
    else if(strcmp(mode,"unavailable")==0)f->types[1].available=false;
    else if(strcmp(mode,"maximum-chain")==0) {
        count=UMI_AI_WORKFLOW_MAX_NODES;d->node_count=count;d->link_count=count-1U;f->outputs[0]=count-1U;
        for(size_t n=0U;n<count;++n) {
            snprintf(f->nodes[n].node_id,sizeof(f->nodes[n].node_id),"step-%02zu",n);
            f->nodes[n].type_index=n==0U?0U:1U;f->nodes[n].supplied_inputs=0U;
            m->costs[n]=(UmiAiWorkflowNodeMemory){.output_bytes={1U},.known_outputs=1U,.scratch_known=true};
            if(n!=0U)f->links[n-1U]=(UmiAiWorkflowLink){n-1U,0U,n,0U};
        }
    } else recognized=false;
    CHECK(recognized);
    UmiStatus status=umi_ai_workflow_memory_plan(strcmp(mode,"null-description")==0?NULL:d,
        strcmp(mode,"null-costs")==0?NULL:m->costs,count,
        strcmp(mode,"null-policy")==0?NULL:&m->policy,effects,
        strcmp(mode,"null-output")==0?NULL:&m->plan,
        strcmp(mode,"null-diagnostic")==0?NULL:&m->diagnostic);
    CHECK(status==expected);
    if(status!=UMI_STATUS_OK) {
        CHECK(memcmp(&m->plan,m->before,sizeof(m->plan))==0);
        if(strcmp(mode,"null-diagnostic")!=0) CHECK(m->diagnostic.issue==issue);
        if(strcmp(mode,"invalid-graph")==0) CHECK(m->diagnostic.workflow.issue==UMI_AI_WORKFLOW_ISSUE_LINK_NODE);
        if(strcmp(mode,"unused-cost-slot")==0) CHECK(m->diagnostic.node_index==0U && m->diagnostic.port_index==7U);
        return 0;
    }
    CHECK(m->diagnostic.issue==UMI_AI_WORKFLOW_MEMORY_ISSUE_NONE);
    if(strcmp(mode,"unknown-resident")==0 || strcmp(mode,"unknown-scratch")==0 || strcmp(mode,"selected-unknown-output")==0) {
        CHECK(!m->plan.estimates_complete && m->plan.verdict==UMI_AI_WORKFLOW_MEMORY_INCOMPLETE);
        CHECK(m->plan.remaining_bytes==0U && m->plan.deficit_bytes==0U);
        CHECK(m->plan.peak_bytes==(strcmp(mode,"unknown-resident")==0?160U:strcmp(mode,"unknown-scratch")==0?150U:70U));return 0;
    }
    CHECK(m->plan.estimates_complete);
    if(strcmp(mode,"maximum-integer")==0){CHECK(m->plan.peak_bytes==UINT64_MAX && m->plan.retained_bytes==UINT64_MAX && m->plan.remaining_bytes==0U);return 0;}
    if(strcmp(mode,"zero-cost")==0 || strcmp(mode,"zero-limit")==0 || strcmp(mode,"resident-only")==0) {
        CHECK(m->plan.peak_bytes==m->policy.resident_bytes && m->plan.peak_step==SIZE_MAX);
        CHECK(m->plan.verdict==UMI_AI_WORKFLOW_MEMORY_WITHIN_DECLARED_LIMIT);return 0;
    }
    if(strcmp(mode,"maximum-chain")==0){CHECK(m->plan.workflow.order.count==64U && m->plan.value_count==64U && m->plan.peak_bytes==12U && m->plan.final_live_bytes==11U);return 0;}
    if(strcmp(mode,"dead-port")==0){CHECK(m->plan.peak_bytes==315U && m->plan.peak_step==0U && m->plan.values[1].last_step==0U && m->plan.steps[0].released_bytes==200U);return 0;}
    if(strcmp(mode,"fanout")==0){CHECK(m->plan.peak_bytes==230U && m->plan.peak_step==2U && m->plan.values[0].last_step==2U && m->plan.steps[1].released_bytes==0U && m->plan.steps[2].released_bytes==100U && m->plan.final_live_bytes==10U);return 0;}
    CHECK(m->plan.peak_bytes==170U && m->plan.peak_step==1U);
    if(strcmp(mode,"retained-result")==0){CHECK(m->plan.workflow.order.count==2U && m->plan.retained_bytes==40U && m->plan.final_live_bytes==50U && m->plan.values[1].retained_output);return 0;}
    if(strcmp(mode,"selected-ancestor")==0){CHECK(m->plan.retained_bytes==100U && m->plan.final_live_bytes==110U && m->plan.values[0].retained_output && m->plan.values[0].last_step==2U);return 0;}
    CHECK(m->plan.retained_bytes==0U && m->plan.final_live_bytes==10U);
    CHECK(m->plan.steps[0].live_after_bytes==110U && m->plan.steps[1].live_after_bytes==50U);
    CHECK(m->plan.values[0].last_step==1U && m->plan.values[1].last_step==2U);
    CHECK(m->plan.workflow.execution_ready==(strcmp(mode,"unavailable")!=0));
    if(strcmp(mode,"over-limit")==0){CHECK(m->plan.verdict==UMI_AI_WORKFLOW_MEMORY_EXCEEDS_LIMIT && m->plan.deficit_bytes==20U && m->plan.remaining_bytes==0U);}
    else CHECK(m->plan.verdict==UMI_AI_WORKFLOW_MEMORY_WITHIN_DECLARED_LIMIT && m->plan.remaining_bytes==m->policy.byte_limit-170U);
    if(strcmp(mode,"copied-lifetime")==0){memset(f,0,sizeof(*f));memset(m->costs,0,sizeof(m->costs));CHECK(strcmp(m->plan.workflow.order.step_ids[0],"z-source")==0 && m->plan.values[0].bytes==100U);}
    if(strcmp(mode,"repeated")==0) for(size_t i=0U;i<100U;++i)CHECK(umi_ai_workflow_memory_plan(d,m->costs,count,&m->policy,effects,&m->plan,&m->diagnostic)==UMI_STATUS_OK && m->plan.final_live_bytes==10U);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    WorkflowFixture *f=workflow_fixture_create();MemoryFixture *m=calloc(1U,sizeof(*m));
    if(f==NULL || m==NULL){free(m);free(f);return 2;}
    int result=exercise(f,m,argv[1]);free(m);free(f);return result;
}
