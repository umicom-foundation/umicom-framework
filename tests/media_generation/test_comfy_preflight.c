/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_preflight.c
 *
 * PURPOSE:
 *   Verify catalogue-backed workflow diagnostics, immutable identities and refusal semantics without provider execution.
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
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;}}while(0)
static UmiProviderConnection Connection(void)
{
    UmiProviderConnection c={0};strcpy(c.id,"comfy-preflight");strcpy(c.provider_id,"comfyui");
    strcpy(c.label,"Preflight fixture");strcpy(c.endpoint,"http://127.0.0.1:8188");
    c.route=UMI_PROVIDER_CONNECTION_LOOPBACK;c.timeout_ms=30000;c.enabled=true;return c;
}
static UmiStatus Prompt(const UmiProviderConnection *c,uint64_t revision,const char *json,UmiComfyPrompt **out)
{
    UmiComfyPromptDraft draft={0};draft.workflow_json=json;draft.workflow_length=strlen(json);
    strcpy(draft.client_id,"preflight-fixture");return UmiComfyPromptCreate(c,revision,&draft,NULL,out);
}
static int Digest(const void *bytes,size_t length,unsigned char *out)
{
    UmiSha256 state;UmiSha256Init(&state);
    return UmiSha256Update(&state,bytes,length)==UMI_STATUS_OK && UmiSha256Final(&state,out)==UMI_STATUS_OK;
}
static char *ManyNodes(size_t count)
{
    size_t capacity=count*64U+4U,used=1U;char *json=malloc(capacity);if(!json)return NULL;
    json[0]='{';
    for(size_t i=0U;i<count;++i){int n=snprintf(json+used,capacity-used,"%s\"n%zu\":{\"class_type\":\"N\",\"inputs\":{}}",i?",":"",i);
        if(n<0 || (size_t)n>=capacity-used){free(json);return NULL;}used+=(size_t)n;}
    strcpy(json+used,"}");return json;
}
static char *ManyRequired(void)
{
    size_t capacity=16384U,used=0U;char *json=malloc(capacity);if(!json)return NULL;
    strcpy(json,"{\"N\":{\"input\":{\"required\":{");used=strlen(json);
    for(size_t i=0U;i<256U;++i){int n=snprintf(json+used,capacity-used,"%s\"p%zu\":[\"STRING\"]",i?",":"",i);
        if(n<0 || (size_t)n>=capacity-used){free(json);return NULL;}used+=(size_t)n;}
    strcpy(json+used,"}},\"output\":[]}}");return json;
}
static const struct {const char *mode,*workflow,*catalogue;UmiComfyPreflightSummary expected;} fixtures[]={
    {"complete","{\"1\":{\"class_type\":\"N\",\"inputs\":{\"text\":\"hello\"}}}","{\"N\":{\"input\":{\"required\":{\"text\":[\"STRING\"]}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}},
    {"missing-class","{\"1\":{\"class_type\":\"Other\",\"inputs\":{}}}","{\"N\":{\"input\":{\"required\":{}},\"output\":[]}}",{.node_count=1U,.known_node_count=0U,.unknown_node_count=1U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=0U,.issue_count=1U}},
    {"empty-catalogue","{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}","{}",{.node_count=1U,.known_node_count=0U,.unknown_node_count=1U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=0U,.issue_count=1U}},
    {"missing-input","{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}","{\"N\":{\"input\":{\"required\":{\"text\":[\"STRING\"]}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=1U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=1U}},
    {"default-not-supplied","{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}","{\"N\":{\"input\":{\"required\":{\"text\":[\"STRING\",{\"default\":\"hello\"}]}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=1U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=1U}},
    {"present-null","{\"1\":{\"class_type\":\"N\",\"inputs\":{\"text\":null}}}","{\"N\":{\"input\":{\"required\":{\"text\":[\"STRING\"]}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}},
    {"opaque-value","{\"1\":{\"class_type\":\"N\",\"inputs\":{\"text\":{\"custom\":true}}}}","{\"N\":{\"input\":{\"required\":{\"text\":{\"customDescriptor\":true}}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}},
    {"optional-absent","{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}","{\"N\":{\"input\":{\"optional\":{\"seed\":[\"INT\"]}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}},
    {"hidden-absent","{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}","{\"N\":{\"input\":{\"hidden\":{\"context\":\"PROMPT\"}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}},
    {"extra-input","{\"1\":{\"class_type\":\"N\",\"inputs\":{\"extra\":13}}}","{\"N\":{\"input\":{\"required\":{}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}},
    {"value-types-not-checked","{\"1\":{\"class_type\":\"N\",\"inputs\":{\"text\":42}}}","{\"N\":{\"input\":{\"required\":{\"text\":[\"STRING\"]}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}},
    {"links-not-checked","{\"1\":{\"class_type\":\"N\",\"inputs\":{\"text\":[\"absent\",999]}}}","{\"N\":{\"input\":{\"required\":{\"text\":[\"STRING\"]}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}},
    {"cycles-not-checked","{\"1\":{\"class_type\":\"N\",\"inputs\":{\"text\":[\"1\",0]}}}","{\"N\":{\"input\":{\"required\":{\"text\":[\"STRING\"]}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}},
    {"flags-unknown","{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}","{\"N\":{\"input\":{\"required\":{}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}},
    {"flags-false","{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}","{\"N\":{\"input\":{\"required\":{}},\"output\":[],\"deprecated\":false,\"experimental\":false,\"api_node\":false,\"output_node\":false}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=0U,.issue_count=0U}},
    {"all-flags","{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}","{\"N\":{\"input\":{\"required\":{}},\"output\":[],\"deprecated\":true,\"experimental\":true,\"api_node\":true,\"output_node\":true}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=1U,.experimental_count=1U,.api_node_count=1U,.observed_output_count=1U,.unknown_output_flag_count=0U,.issue_count=3U}},
    {"output-unconnected","{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}","{\"N\":{\"input\":{\"required\":{}},\"output\":[\"IMAGE\"],\"output_node\":true}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=1U,.unknown_output_flag_count=0U,.issue_count=0U}},
    {"unicode","{\"节点\":{\"class_type\":\"示例\",\"inputs\":{\"文本\":\"你好\"}}}","{\"示例\":{\"input\":{\"required\":{\"文本\":[\"STRING\"]}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}},
    {"escaped-key","{\"1\":{\"class_type\":\"N\",\"inputs\":{\"te\\u0078t\":\"hello\"}}}","{\"N\":{\"input\":{\"required\":{\"text\":[\"STRING\"]}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}},
    {"all-nodes-inspected","{\"later\":{\"class_type\":\"N\",\"inputs\":{\"text\":\"hello\"}},\"unused\":{\"class_type\":\"N\",\"inputs\":{}}}","{\"N\":{\"input\":{\"required\":{\"text\":[\"STRING\"]}},\"output\":[]}}",{.node_count=2U,.known_node_count=2U,.unknown_node_count=0U,.missing_required_count=1U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=2U,.issue_count=1U}},
    {"multiple-missing","{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}","{\"N\":{\"input\":{\"required\":{\"a\":[\"STRING\"],\"b\":[\"INT\"]}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=2U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=2U}},
    {"opaque-model-name","{\"1\":{\"class_type\":\"N\",\"inputs\":{\"model\":\"not-installed.bin\"}}}","{\"N\":{\"input\":{\"required\":{\"model\":[[\"some-other.bin\"]]}},\"output\":[]}}",{.node_count=1U,.known_node_count=1U,.unknown_node_count=0U,.missing_required_count=0U,.deprecated_count=0U,.experimental_count=0U,.api_node_count=0U,.observed_output_count=0U,.unknown_output_flag_count=1U,.issue_count=0U}}
};
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    UmiProviderConnection c=Connection(),observed=c,current=c;
    UmiComfyPrompt *prompt=NULL;UmiComfyNodeCatalogue *catalogue=NULL;
    UmiComfyPreflightReport *report=NULL;
    const char *workflow="{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}";
    const char *schema="{\"N\":{\"input\":{},\"output\":[]}}";
    size_t fixture=SIZE_MAX;char *owned_workflow=NULL,*owned_schema=NULL;
    for(size_t i=0U;i<sizeof fixtures/sizeof fixtures[0];++i)if(!strcmp(mode,fixtures[i].mode)){
        fixture=i;workflow=fixtures[i].workflow;schema=fixtures[i].catalogue;break;
    }
    if(!strcmp(mode,"diagnostic-limit") || !strcmp(mode,"diagnostic-truncation")){
        owned_workflow=ManyNodes(!strcmp(mode,"diagnostic-limit")?1U:2U);owned_schema=ManyRequired();
        CHECK(owned_workflow && owned_schema);workflow=owned_workflow;schema=owned_schema;
    }else if(!strcmp(mode,"maximum-nodes")){
        owned_workflow=ManyNodes(1024U);CHECK(owned_workflow);workflow=owned_workflow;
    }else if(!strcmp(mode,"maximum-bytes")){
        size_t n=strlen(workflow);owned_workflow=malloc(UMI_COMFY_PROMPT_MAX_BYTES+1U);CHECK(owned_workflow);
        memset(owned_workflow,' ',UMI_COMFY_PROMPT_MAX_BYTES);memcpy(owned_workflow,workflow,n);
        owned_workflow[UMI_COMFY_PROMPT_MAX_BYTES]=0;workflow=owned_workflow;
    }else if(!strcmp(mode,"source-order")){
        workflow="{\"b\":{\"class_type\":\"N\",\"inputs\":{}},\"a\":{\"class_type\":\"Unknown\",\"inputs\":{}}}";
        schema="{\"N\":{\"input\":{\"required\":{\"z\":[\"STRING\"],\"a\":[\"INT\"]}},\"output\":[],\"deprecated\":true,\"experimental\":true,\"api_node\":true}}";
    }else if(!strcmp(mode,"workflow-span"))workflow=" \n{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}\t ";
    if(!strcmp(mode,"catalogue-settings"))strcpy(observed.label,"Different catalogue observation");
    CHECK(Prompt(&c,7U,workflow,&prompt)==UMI_STATUS_OK);
    CHECK(UmiComfyNodeCatalogueCreate(&observed,!strcmp(mode,"catalogue-revision")?8U:7U,schema,strlen(schema),NULL,&catalogue)==UMI_STATUS_OK);
    UmiStatus expected=UMI_STATUS_OK;uint64_t revision=7U;
    if(!strcmp(mode,"current-revision")){revision=8U;expected=UMI_STATUS_BUSY;}
    if(!strcmp(mode,"zero-revision")){revision=0U;expected=UMI_STATUS_BUSY;}
    if(!strcmp(mode,"catalogue-revision") || !strcmp(mode,"catalogue-settings"))expected=UMI_STATUS_BUSY;
    if(!strcmp(mode,"current-settings")){strcpy(current.label,"Changed saved label");expected=UMI_STATUS_BUSY;}
    if(!strcmp(mode,"disabled")){current.enabled=false;expected=UMI_STATUS_PERMISSION_DENIED;}
    UmiCancellationToken *cancel=NULL;
    if(!strcmp(mode,"cancel")){CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);umi_cancellation_token_request(cancel);expected=UMI_STATUS_CANCELLED;}
    CHECK(UmiComfyPreflightCreate(&current,revision,prompt,catalogue,cancel,&report)==expected);
    umi_cancellation_token_destroy(cancel);
    if(expected!=UMI_STATUS_OK){CHECK(report==NULL);goto done;}
    CHECK(report!=NULL);
    UmiComfyPreflightSummary summary;CHECK(UmiComfyPreflightInspect(report,&summary)==UMI_STATUS_OK);
    CHECK(summary.issue_count==summary.stored_issue_count+summary.omitted_issue_count);
    CHECK(summary.all_node_requirements_observed==(summary.unknown_node_count==0U && summary.missing_required_count==0U));
    if(fixture!=SIZE_MAX){
        CHECK(summary.node_count==fixtures[fixture].expected.node_count);
        CHECK(summary.known_node_count==fixtures[fixture].expected.known_node_count);
        CHECK(summary.unknown_node_count==fixtures[fixture].expected.unknown_node_count);
        CHECK(summary.missing_required_count==fixtures[fixture].expected.missing_required_count);
        CHECK(summary.deprecated_count==fixtures[fixture].expected.deprecated_count);
        CHECK(summary.experimental_count==fixtures[fixture].expected.experimental_count);
        CHECK(summary.api_node_count==fixtures[fixture].expected.api_node_count);
        CHECK(summary.observed_output_count==fixtures[fixture].expected.observed_output_count);
        CHECK(summary.unknown_output_flag_count==fixtures[fixture].expected.unknown_output_flag_count);
        CHECK(summary.issue_count==fixtures[fixture].expected.issue_count);
    }else if(!strcmp(mode,"diagnostic-limit") || !strcmp(mode,"diagnostic-truncation")){
        size_t total=!strcmp(mode,"diagnostic-limit")?256U:512U;
        CHECK(summary.issue_count==total && summary.missing_required_count==total);
        CHECK(summary.stored_issue_count==256U && summary.omitted_issue_count==total-256U);
        UmiComfyPreflightIssue issue;CHECK(UmiComfyPreflightIssueAt(report,255U,&issue)==UMI_STATUS_OK);
        CHECK(!strcmp(issue.input_name,"p255") && !strcmp(issue.node_id,"n0"));
    }else if(!strcmp(mode,"maximum-nodes")){CHECK(summary.node_count==1024U && summary.known_node_count==1024U);}
    else if(!strcmp(mode,"maximum-bytes")){CHECK(summary.all_node_requirements_observed);}
    else if(!strcmp(mode,"source-order")){
        static const UmiComfyPreflightIssueKind kinds[]={UMI_COMFY_PREFLIGHT_INPUT_MISSING,UMI_COMFY_PREFLIGHT_INPUT_MISSING,
            UMI_COMFY_PREFLIGHT_DEPRECATED,UMI_COMFY_PREFLIGHT_EXPERIMENTAL,UMI_COMFY_PREFLIGHT_API_NODE,UMI_COMFY_PREFLIGHT_CLASS_MISSING};
        CHECK(summary.issue_count==6U);
        for(size_t i=0U;i<6U;++i){UmiComfyPreflightIssue issue;CHECK(UmiComfyPreflightIssueAt(report,i,&issue)==UMI_STATUS_OK);
            CHECK(issue.kind==kinds[i]);CHECK(!strcmp(issue.node_id,i==5U?"a":"b"));
            if(i<2U)CHECK(!strcmp(issue.input_name,i?"a":"z"));else CHECK(issue.input_name[0]==0);}
    }else if(!strcmp(mode,"workflow-span")){
        const char *span=NULL;size_t length=0U;
        CHECK(UmiComfyPromptWorkflow(prompt,&span,&length)==UMI_STATUS_OK);
        CHECK(length==strlen(workflow) && memcmp(span,workflow,length)==0 && span[length]=='}');
/* One fixture checks the three related identity bindings together; separate labels did not exercise different behaviour. The former implementation is retained for owner review. */
#if 0
    }else if(!strcmp(mode,"request-identity") || !strcmp(mode,"catalogue-identity") || !strcmp(mode,"connection-identity")){
#endif
    }else if(!strcmp(mode,"byte-identities")){
        UmiComfyPromptReview review;const char *body=NULL;unsigned char digest[UMI_SHA256_BYTES];
        CHECK(UmiComfyPromptInspect(prompt,&review,&body)==UMI_STATUS_OK);
        CHECK(Digest(body,review.body_bytes,digest));CHECK(memcmp(digest,summary.request_sha256,sizeof digest)==0);
        CHECK(Digest(schema,strlen(schema),digest));CHECK(memcmp(digest,summary.catalogue_sha256,sizeof digest)==0);
        CHECK(UmiProviderConnectionFingerprint(&c,digest)==UMI_STATUS_OK);
        CHECK(memcmp(digest,summary.connection_sha256,sizeof digest)==0 && summary.connection_revision==7U);
    }else if(!strcmp(mode,"catalogue-spelling")){
        UmiComfyNodeCatalogue *other=NULL;UmiComfyNodeCatalogueReview a,b;
        char padded[256];CHECK(snprintf(padded,sizeof padded," %s ",schema)>0);
        CHECK(UmiComfyNodeCatalogueCreate(&c,7U,padded,strlen(padded),NULL,&other)==UMI_STATUS_OK);
        CHECK(UmiComfyNodeCatalogueInspect(catalogue,&a)==UMI_STATUS_OK);
        CHECK(UmiComfyNodeCatalogueInspect(other,&b)==UMI_STATUS_OK);
        CHECK(memcmp(a.response_sha256,b.response_sha256,UMI_SHA256_BYTES)!=0);
        CHECK(memcmp(a.connection_sha256,b.connection_sha256,UMI_SHA256_BYTES)==0);
        UmiComfyNodeCatalogueDestroy(other);
    }else if(!strcmp(mode,"owner-lifetime")){
        UmiComfyPromptDestroy(prompt);prompt=NULL;UmiComfyNodeCatalogueDestroy(catalogue);catalogue=NULL;
        UmiComfyPreflightSummary after;CHECK(UmiComfyPreflightInspect(report,&after)==UMI_STATUS_OK);
/* Compare public fields and digests after owner destruction; structure padding is not part of the portable value contract. The former implementation is retained for owner review. */
#if 0
        CHECK(memcmp(&summary,&after,sizeof summary)==0);
#endif
        CHECK(summary.node_count==after.node_count);
        CHECK(summary.known_node_count==after.known_node_count);
        CHECK(summary.unknown_node_count==after.unknown_node_count);
        CHECK(summary.missing_required_count==after.missing_required_count);
        CHECK(summary.deprecated_count==after.deprecated_count);
        CHECK(summary.experimental_count==after.experimental_count);
        CHECK(summary.api_node_count==after.api_node_count);
        CHECK(summary.observed_output_count==after.observed_output_count);
        CHECK(summary.unknown_output_flag_count==after.unknown_output_flag_count);
        CHECK(summary.issue_count==after.issue_count);
        CHECK(summary.connection_revision==after.connection_revision);
        CHECK(memcmp(summary.request_sha256,after.request_sha256,UMI_SHA256_BYTES)==0);
        CHECK(memcmp(summary.catalogue_sha256,after.catalogue_sha256,UMI_SHA256_BYTES)==0);
        CHECK(memcmp(summary.connection_sha256,after.connection_sha256,UMI_SHA256_BYTES)==0);
    }else if(!strcmp(mode,"copied-summary")){
        memset(&summary,0xa5,sizeof summary);CHECK(UmiComfyPreflightInspect(report,&summary)==UMI_STATUS_OK);
        CHECK(summary.node_count==1U && summary.connection_revision==7U);
    }else if(!strcmp(mode,"occupied")){
        UmiComfyPreflightReport *before=report;
        CHECK(UmiComfyPreflightCreate(&c,7U,prompt,catalogue,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);CHECK(report==before);
    }else if(!strcmp(mode,"accessor-refusal")){
        UmiComfyPreflightSummary before;memset(&summary,0xa5,sizeof summary);memcpy(&before,&summary,sizeof before);
        CHECK(UmiComfyPreflightInspect(NULL,&summary)==UMI_STATUS_INVALID_ARGUMENT);CHECK(memcmp(&before,&summary,sizeof before)==0);
        UmiComfyPreflightIssue issue,prior;memset(&issue,0xa5,sizeof issue);memcpy(&prior,&issue,sizeof prior);
        CHECK(UmiComfyPreflightIssueAt(report,0U,&issue)==UMI_STATUS_NOT_FOUND);CHECK(memcmp(&prior,&issue,sizeof prior)==0);
        CHECK(UmiComfyPreflightIssueAt(report,SIZE_MAX,&issue)==UMI_STATUS_NOT_FOUND);CHECK(memcmp(&prior,&issue,sizeof prior)==0);
        CHECK(UmiComfyPreflightIssueAt(NULL,0U,&issue)==UMI_STATUS_INVALID_ARGUMENT);CHECK(memcmp(&prior,&issue,sizeof prior)==0);
    }else if(!strcmp(mode,"nulls")){
        UmiComfyPreflightReport *none=NULL;
        CHECK(UmiComfyPreflightCreate(NULL,7U,prompt,catalogue,NULL,&none)==UMI_STATUS_INVALID_ARGUMENT && none==NULL);
        CHECK(UmiComfyPreflightCreate(&c,7U,NULL,catalogue,NULL,&none)==UMI_STATUS_INVALID_ARGUMENT && none==NULL);
        CHECK(UmiComfyPreflightCreate(&c,7U,prompt,NULL,NULL,&none)==UMI_STATUS_INVALID_ARGUMENT && none==NULL);
        CHECK(UmiComfyPreflightCreate(&c,7U,prompt,catalogue,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyPreflightInspect(report,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyPreflightIssueAt(report,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        UmiComfyPreflightDestroy(NULL);
    }else if(!strcmp(mode,"observation-refusal")){
        const char *span="sentinel",*prior=span;size_t length=71U;
        CHECK(UmiComfyPromptWorkflow(NULL,&span,&length)==UMI_STATUS_INVALID_ARGUMENT && span==prior && length==71U);
        CHECK(UmiComfyPromptWorkflow(prompt,NULL,&length)==UMI_STATUS_INVALID_ARGUMENT && length==71U);
        CHECK(UmiComfyPromptWorkflow(prompt,&span,NULL)==UMI_STATUS_INVALID_ARGUMENT && span==prior);
        UmiComfyNodeCatalogueReview review,before;memset(&review,0xa5,sizeof review);memcpy(&before,&review,sizeof before);
        CHECK(UmiComfyNodeCatalogueInspect(NULL,&review)==UMI_STATUS_INVALID_ARGUMENT && memcmp(&review,&before,sizeof review)==0);
        CHECK(UmiComfyNodeCatalogueInspect(catalogue,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }else CHECK(0);
done:
    UmiComfyPreflightDestroy(report);UmiComfyPromptDestroy(prompt);UmiComfyNodeCatalogueDestroy(catalogue);
    free(owned_workflow);free(owned_schema);return 0;
}
