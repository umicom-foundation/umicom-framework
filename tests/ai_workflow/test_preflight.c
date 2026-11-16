/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workflow/test_preflight.c
 *
 * PURPOSE:
 *   Exercise typed dependency admission, selected closure, permission requirements and retained outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"

#define CHECK(condition) do { if (!(condition)) {     fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while (0)

static int expect_refusal(WorkflowFixture *f, uint32_t effects,
    UmiStatus expected_status, UmiAiWorkflowIssue expected_issue)
{
    UmiStatus status = umi_ai_workflow_preflight(&f->description,effects,&f->plan,&f->diagnostic);
    CHECK(status == expected_status);
    CHECK(f->diagnostic.issue == expected_issue);
    CHECK(memcmp(&f->plan,f->before,sizeof(f->plan)) == 0);
    CHECK(strlen(umi_ai_workflow_issue_text(f->diagnostic.issue)) > 0U);
    return 0;
}

static int run_case(WorkflowFixture *f, const char *name)
{
    UmiAiWorkflowDescription *d = &f->description;
    UmiStatus status;
    if (strcmp(name,"valid_chain")==0 || strcmp(name,"dependency_before_alphabetic")==0) {
        CHECK(umi_ai_workflow_preflight(d,0U,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
        CHECK(f->plan.order.count==3U && f->plan.execution_ready);
        CHECK(f->diagnostic.issue==UMI_AI_WORKFLOW_ISSUE_NONE);
        CHECK(f->diagnostic.node_index==SIZE_MAX);
        for(size_t i=0U;i<3U;++i) CHECK(f->plan.node_indices[i]==i);
        CHECK(strcmp(f->plan.order.step_ids[0],"z-source")==0);
        CHECK(strcmp(f->plan.order.step_ids[2],"a-output")==0);
        return 0;
    }
    if (strcmp(name,"missing_node")==0) {
        f->links[0].source_node=SIZE_MAX;
        return expect_refusal(f,0U,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_LINK_NODE);
    }
    if (strcmp(name,"missing_source_port")==0 || strcmp(name,"missing_target_port")==0) {
        if(strcmp(name,"missing_source_port")==0) f->links[0].source_port=SIZE_MAX;
        else f->links[0].target_port=SIZE_MAX;
        CHECK(expect_refusal(f,0U,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_PORT)==0);
        CHECK(f->diagnostic.link_index==0U && f->diagnostic.port_index==SIZE_MAX);
        return 0;
    }
    if(strcmp(name,"type_mismatch")==0) {
        f->types[1].inputs[0].kind=UMI_AI_WORKFLOW_IMAGE;
        return expect_refusal(f,0U,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_TYPE_MISMATCH);
    }
    if(strcmp(name,"duplicate_binding")==0 || strcmp(name,"external_link_conflict")==0) {
        if(strcmp(name,"duplicate_binding")==0) {f->links[2]=f->links[0];d->link_count=3U;}
        else f->nodes[1].supplied_inputs=1U;
        return expect_refusal(f,0U,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_BINDING);
    }
    if(strcmp(name,"required_input")==0 || strcmp(name,"optional_input")==0 ||
       strcmp(name,"external_input")==0) {
        f->links[0]=f->links[1];d->link_count=1U;
        if(strcmp(name,"required_input")==0)
            return expect_refusal(f,0U,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_REQUIRED_INPUT);
        if(strcmp(name,"optional_input")==0) f->types[1].inputs[0].required=false;
        else f->nodes[1].supplied_inputs=1U;
        CHECK(umi_ai_workflow_preflight(d,0U,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
        CHECK(f->plan.order.count==2U && f->plan.node_indices[0]==1U);
        return 0;
    }
    if(strcmp(name,"self_cycle")==0 || strcmp(name,"cycle")==0 || strcmp(name,"unselected_cycle")==0) {
        if(strcmp(name,"self_cycle")==0) f->links[0].source_node=1U;
        else {
            f->nodes[3]=(UmiAiWorkflowNode){"cycle-left",1U,0U};
            f->nodes[4]=(UmiAiWorkflowNode){"cycle-right",1U,0U};
            d->node_count=5U;
            f->links[2]=(UmiAiWorkflowLink){3U,0U,4U,0U};
            f->links[3]=(UmiAiWorkflowLink){4U,0U,3U,0U};
            d->link_count=4U;
            if(strcmp(name,"cycle")==0) f->outputs[0]=3U;
        }
        return expect_refusal(f,0U,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_CYCLE);
    }
    if(strcmp(name,"too_many_nodes")==0 || strcmp(name,"too_many_links")==0 ||
       strcmp(name,"too_many_types")==0 || strcmp(name,"too_many_ports")==0 ||
       strcmp(name,"too_many_outputs")==0) {
        if(strcmp(name,"too_many_nodes")==0) d->node_count=SIZE_MAX;
        if(strcmp(name,"too_many_links")==0) d->link_count=SIZE_MAX;
        if(strcmp(name,"too_many_types")==0) d->type_count=SIZE_MAX;
        if(strcmp(name,"too_many_ports")==0) f->types[1].input_count=SIZE_MAX;
        if(strcmp(name,"too_many_outputs")==0) d->output_count=SIZE_MAX;
        return expect_refusal(f,0U,UMI_STATUS_CAPACITY_EXCEEDED,UMI_AI_WORKFLOW_ISSUE_CAPACITY);
    }
    if(strcmp(name,"empty_graph")==0 || strcmp(name,"zero_outputs")==0 ||
       strcmp(name,"null_link_storage")==0 || strcmp(name,"null_type_storage")==0 ||
       strcmp(name,"null_node_storage")==0 || strcmp(name,"null_output_storage")==0) {
        if(strcmp(name,"empty_graph")==0) d->node_count=0U;
        if(strcmp(name,"zero_outputs")==0) d->output_count=0U;
        if(strcmp(name,"null_link_storage")==0) d->links=NULL;
        if(strcmp(name,"null_type_storage")==0) d->types=NULL;
        if(strcmp(name,"null_node_storage")==0) d->nodes=NULL;
        if(strcmp(name,"null_output_storage")==0) d->output_nodes=NULL;
        return expect_refusal(f,0U,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_ARGUMENT);
    }
    if(strcmp(name,"null_description")==0 || strcmp(name,"null_plan")==0 ||
       strcmp(name,"null_diagnostic")==0) {
        status=umi_ai_workflow_preflight(strcmp(name,"null_description")==0?NULL:d,
            0U,strcmp(name,"null_plan")==0?NULL:&f->plan,
            strcmp(name,"null_diagnostic")==0?NULL:&f->diagnostic);
        CHECK(status==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&f->plan,f->before,sizeof(f->plan))==0);
        return 0;
    }
    if(strcmp(name,"unterminated_identifier")==0 || strcmp(name,"unsafe_identifier")==0) {
        if(strcmp(name,"unterminated_identifier")==0)
            memset(f->nodes[2].node_id,'x',sizeof(f->nodes[2].node_id));
        else strcpy(f->types[1].inputs[0].name,"../input file");
        return expect_refusal(f,0U,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_IDENTIFIER);
    }
    if(strcmp(name,"duplicate_node_id")==0 || strcmp(name,"duplicate_operation_id")==0 ||
       strcmp(name,"duplicate_port_name")==0) {
        if(strcmp(name,"duplicate_node_id")==0) strcpy(f->nodes[2].node_id,f->nodes[0].node_id);
        if(strcmp(name,"duplicate_operation_id")==0) strcpy(f->types[2].operation_id,f->types[0].operation_id);
        if(strcmp(name,"duplicate_port_name")==0) {
            f->types[1].inputs[1]=f->types[1].inputs[0];f->types[1].input_count=2U;
        }
        return expect_refusal(f,0U,UMI_STATUS_ALREADY_EXISTS,UMI_AI_WORKFLOW_ISSUE_DUPLICATE);
    }
    if(strcmp(name,"unknown_value_kind")==0) {
        f->types[0].outputs[0].kind=(UmiAiWorkflowValueKind)999;
        return expect_refusal(f,0U,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_VALUE_KIND);
    }
    if(strcmp(name,"unknown_type_index")==0) {
        f->nodes[1].type_index=SIZE_MAX;
        return expect_refusal(f,0U,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_NODE_TYPE);
    }
    if(strcmp(name,"invalid_supplied_mask")==0) {
        f->nodes[1].supplied_inputs=UINT64_C(1)<<63U;
        return expect_refusal(f,0U,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_BINDING);
    }
    if(strcmp(name,"effect_denied")==0 || strcmp(name,"effect_allowed")==0) {
        f->types[2].effects=UMI_AI_WORKFLOW_EFFECT_WRITE_FILE|UMI_AI_WORKFLOW_EFFECT_NETWORK;
        if(strcmp(name,"effect_denied")==0)
            return expect_refusal(f,UMI_AI_WORKFLOW_EFFECT_WRITE_FILE,
                UMI_STATUS_PERMISSION_DENIED,UMI_AI_WORKFLOW_ISSUE_EFFECT_DENIED);
        CHECK(umi_ai_workflow_preflight(d,UMI_AI_WORKFLOW_KNOWN_EFFECTS,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
        CHECK(f->plan.required_effects==f->types[2].effects);
        return 0;
    }
    if(strcmp(name,"unknown_policy_bit")==0 || strcmp(name,"unknown_effect_definition")==0) {
        uint32_t policy=UINT32_C(0x80000000);
        if(strcmp(name,"unknown_effect_definition")==0) {f->types[0].effects=policy;policy=0U;}
        return expect_refusal(f,policy,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_EFFECT_DEFINITION);
    }
    if(strcmp(name,"unavailable_provider")==0) {
        f->types[0].available=false;f->types[2].available=false;
        CHECK(umi_ai_workflow_preflight(d,0U,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
        CHECK(!f->plan.execution_ready && f->plan.unavailable_count==2U);
        CHECK(f->plan.unavailable_indices[0]==0U && f->plan.unavailable_indices[1]==2U);
        return 0;
    }
    if(strcmp(name,"inactive_unavailable")==0 || strcmp(name,"inactive_required_input")==0 ||
       strcmp(name,"inactive_effect")==0) {
        f->outputs[0]=0U;d->link_count=0U;d->links=NULL;
        if(strcmp(name,"inactive_unavailable")==0) f->types[1].available=false;
        if(strcmp(name,"inactive_effect")==0) f->types[1].effects=UMI_AI_WORKFLOW_EFFECT_PROCESS;
        CHECK(umi_ai_workflow_preflight(d,0U,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
        CHECK(f->plan.order.count==1U && f->plan.node_indices[0]==0U);
        CHECK(f->plan.execution_ready && f->plan.required_effects==0U);
        return 0;
    }
    if(strcmp(name,"invalid_output_index")==0 || strcmp(name,"duplicate_output")==0) {
        if(strcmp(name,"invalid_output_index")==0) f->outputs[0]=SIZE_MAX;
        else {f->outputs[1]=f->outputs[0];d->output_count=2U;}
        return expect_refusal(f,0U,UMI_STATUS_INVALID_ARGUMENT,UMI_AI_WORKFLOW_ISSUE_OUTPUT);
    }
    if(strcmp(name,"multiple_outputs")==0) {
        f->outputs[1]=0U;d->output_count=2U;
        CHECK(umi_ai_workflow_preflight(d,0U,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
        CHECK(f->plan.order.count==3U);
        return 0;
    }
    if(strcmp(name,"parallel_port_links")==0) {
        f->types[1].inputs[1]=(UmiAiWorkflowPort){"style",UMI_AI_WORKFLOW_TEXT,true};
        f->types[1].input_count=2U;
        f->links[2]=(UmiAiWorkflowLink){0U,0U,1U,1U};d->link_count=3U;
        CHECK(umi_ai_workflow_preflight(d,0U,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
        CHECK(f->plan.order.count==3U && f->plan.node_indices[1]==1U);
        return 0;
    }
    if(strcmp(name,"deterministic_order")==0) {
        d->link_count=0U;d->links=NULL;f->nodes[1].supplied_inputs=1U;f->nodes[2].supplied_inputs=1U;
        f->outputs[0]=0U;f->outputs[1]=1U;f->outputs[2]=2U;d->output_count=3U;
        CHECK(umi_ai_workflow_preflight(d,0U,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
        CHECK(f->plan.node_indices[0]==2U && f->plan.node_indices[1]==1U && f->plan.node_indices[2]==0U);
        uint64_t fingerprint=f->plan.order.fingerprint;
        f->outputs[0]=2U;f->outputs[2]=0U;
        CHECK(umi_ai_workflow_preflight(d,0U,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
        CHECK(f->plan.order.fingerprint==fingerprint && f->plan.node_indices[0]==2U);
        return 0;
    }
    if(strcmp(name,"capacity_chain")==0) {
        d->node_count=UMI_AI_WORKFLOW_MAX_NODES;d->link_count=d->node_count-1U;
        for(size_t n=0U;n<d->node_count;++n) {
            snprintf(f->nodes[n].node_id,sizeof(f->nodes[n].node_id),"node-%02zu",d->node_count-n);
            f->nodes[n].type_index=1U;f->nodes[n].supplied_inputs=n==0U?1U:0U;
            if(n!=0U) f->links[n-1U]=(UmiAiWorkflowLink){n-1U,0U,n,0U};
        }
        f->outputs[0]=d->node_count-1U;
        CHECK(umi_ai_workflow_preflight(d,0U,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
        CHECK(f->plan.order.count==UMI_AI_WORKFLOW_MAX_NODES);
        for(size_t n=0U;n<d->node_count;++n) CHECK(f->plan.node_indices[n]==n);
        return 0;
    }
    if(strcmp(name,"maximum_identifier")==0) {
        memset(f->nodes[0].node_id,'z',sizeof(f->nodes[0].node_id)-1U);
        f->nodes[0].node_id[sizeof(f->nodes[0].node_id)-1U]='\0';
        CHECK(umi_ai_workflow_preflight(d,0U,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
        CHECK(strlen(f->plan.order.step_ids[0])==UMI_AI_WORKFLOW_ID_CAPACITY-1U);
        return 0;
    }
    if(strcmp(name,"unused_slots")==0) {
        memset(&f->types[3],0xa5,sizeof(f->types[3]));
        memset(&f->types[0].inputs[0],0xa5,sizeof(f->types[0].inputs[0]));
        memset(&f->nodes[3],0xa5,sizeof(f->nodes[3]));
        memset(&f->links[2],0xa5,sizeof(f->links[2]));
        CHECK(umi_ai_workflow_preflight(d,0U,&f->plan,&f->diagnostic)==UMI_STATUS_OK);
        return 0;
    }
    if(strcmp(name,"diagnostic_text")==0) {
        CHECK(strcmp(umi_ai_workflow_issue_text((UmiAiWorkflowIssue)999),"Unknown workflow diagnostic")==0);
        for(int i=(int)UMI_AI_WORKFLOW_ISSUE_NONE;i<=(int)UMI_AI_WORKFLOW_ISSUE_GRAPH;++i)
            CHECK(strlen(umi_ai_workflow_issue_text((UmiAiWorkflowIssue)i))>0U);
        return 0;
    }
    fprintf(stderr,"Unknown case: %s\n",name);
    return 1;
}

int main(int argc, char **argv)
{
    if(argc!=2) return 2;
    WorkflowFixture *fixture=workflow_fixture_create();
    if(fixture==NULL) return 2;
    int result=run_case(fixture,argv[1]);
    free(fixture);
    return result;
}
