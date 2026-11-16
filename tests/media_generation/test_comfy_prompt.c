/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_prompt.c
 *
 * PURPOSE:
 *   Exercise exact ComfyUI request ownership, bounded workflow admission and shared receipt identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_prompt.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"check failed at %d: %s\n",__LINE__,#x); return 1; } } while (0)
static const char workflow[]="{\"node\":{\"class_type\":\"ExampleNode\",\"inputs\":{\"value\":7}}}";
static UmiProviderConnection Connection(void)
{
    UmiProviderConnection c={0};
    strcpy(c.id,"local-comfy"); strcpy(c.provider_id,"comfyui");
    strcpy(c.label,"Native request fixture"); strcpy(c.endpoint,"http://127.0.0.1:8188");
    c.route=UMI_PROVIDER_CONNECTION_LOOPBACK; c.timeout_ms=30000; c.enabled=true;
    return c;
}
static char *Nodes(size_t count)
{
    size_t capacity=count*80U+3U;
    char *json=malloc(capacity); if(json==NULL) return NULL;
    size_t used=0; json[used++]='{';
    for(size_t i=0;i<count;++i){
        int n=snprintf(json+used,capacity-used,"%s\"n%zu\":{\"class_type\":\"Example\",\"inputs\":{}}",i?",":"",i);
        if(n<0 || (size_t)n>=capacity-used){free(json);return NULL;}
        used+=(size_t)n;
    }
    json[used++]='}';json[used]='\0';return json;
}
int main(int argc,char **argv)
{
    CHECK(argc==2); const char *mode=argv[1];
    UmiProviderConnection c=Connection(); uint64_t revision=7;
    UmiComfyPromptDraft d={workflow,sizeof(workflow)-1,"fixture-client"};
    UmiComfyPrompt *prompt=NULL; UmiCancellationToken *cancel=NULL;
    char *owned=NULL; UmiStatus expected=UMI_STATUS_OK;
    const char *json=NULL;
    if(!strcmp(mode,"unicode")){strcpy(d.client_id,"café");json="{\"图\":{\"class_type\":\"示例\",\"inputs\":{\"文本\":\"hello\"}}}";}
    else if(!strcmp(mode,"escaped-client"))strcpy(d.client_id,"a\"b\\c\n");
    else if(!strcmp(mode,"custom-values"))json="{\"n\":{\"class_type\":\"Custom\",\"inputs\":{\"opaque\":[\"absent-node\",999],\"config\":{\"flag\":true,\"none\":null}},\"_meta\":{\"title\":\"Custom fixture\"}}}";
    else if(!strcmp(mode,"source-spelling"))json=" \n { \"n\": {\"class_type\":\"Example\",\"inputs\":{\"value\":1.00e2}} } \t";
    else if(!strcmp(mode,"max-client")){memset(d.client_id,'x',128);d.client_id[128]=0;}
    else if(!strcmp(mode,"empty-client")){d.client_id[0]=0;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"unterminated-client")){memset(d.client_id,'x',sizeof(d.client_id));expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"invalid-client-utf8")){d.client_id[0]=(char)0xff;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"zero-revision")){revision=0;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"disabled")){c.enabled=false;expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(mode,"provider")){strcpy(c.provider_id,"different");expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(mode,"remote")){c.route=UMI_PROVIDER_CONNECTION_HTTPS;strcpy(c.endpoint,"https://example.invalid");expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(mode,"path")){strcat(c.endpoint,"/prompt");expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(mode,"trailing-slash")){strcat(c.endpoint,"/");expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(mode,"secret")){strcpy(c.secret_reference,"vault:example");expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"bad-port")){strcpy(c.endpoint,"http://127.0.0.1:80");expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"empty")){json="{}";expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"array")){json="[]";expected=UMI_STATUS_PARSE_ERROR;}
    else if(!strcmp(mode,"node-kind")){json="{\"n\":null}";expected=UMI_STATUS_PARSE_ERROR;}
    else if(!strcmp(mode,"missing-type")){json="{\"n\":{\"inputs\":{}}}";expected=UMI_STATUS_PARSE_ERROR;}
    else if(!strcmp(mode,"empty-type")){json="{\"n\":{\"class_type\":\"\",\"inputs\":{}}}";expected=UMI_STATUS_PARSE_ERROR;}
    else if(!strcmp(mode,"type-kind")){json="{\"n\":{\"class_type\":2,\"inputs\":{}}}";expected=UMI_STATUS_PARSE_ERROR;}
    else if(!strcmp(mode,"missing-inputs")){json="{\"n\":{\"class_type\":\"Example\"}}";expected=UMI_STATUS_PARSE_ERROR;}
    else if(!strcmp(mode,"inputs-kind")){json="{\"n\":{\"class_type\":\"Example\",\"inputs\":[]}}";expected=UMI_STATUS_PARSE_ERROR;}
    else if(!strcmp(mode,"empty-node-id")){json="{\"\":{\"class_type\":\"Example\",\"inputs\":{}}}";expected=UMI_STATUS_PARSE_ERROR;}
    else if(!strcmp(mode,"duplicate")){json="{\"n\":{},\"n\":{}}";expected=UMI_STATUS_ALREADY_EXISTS;}
    else if(!strcmp(mode,"escaped-duplicate")){json="{\"n\":{},\"\\u006e\":{}}";expected=UMI_STATUS_ALREADY_EXISTS;}
    else if(!strcmp(mode,"nested-duplicate")){json="{\"n\":{\"class_type\":\"Example\",\"inputs\":{\"x\":1,\"x\":2}}}";expected=UMI_STATUS_ALREADY_EXISTS;}
    else if(!strcmp(mode,"malformed")){json="{\"n\":}";expected=UMI_STATUS_PARSE_ERROR;}
    else if(!strcmp(mode,"trailing")){json="{} []";expected=UMI_STATUS_PARSE_ERROR;}
    else if(!strcmp(mode,"long-type") || !strcmp(mode,"long-node-id")){
        char text[130];memset(text,'x',129);text[129]=0;owned=malloc(300);CHECK(owned);
        snprintf(owned,300,"{\"%s\":{\"class_type\":\"%s\",\"inputs\":{}}}",!strcmp(mode,"long-node-id")?text:"n",!strcmp(mode,"long-type")?text:"Example");
        json=owned;expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(!strcmp(mode,"max-nodes") || !strcmp(mode,"too-many-nodes")){
        owned=Nodes(UMI_COMFY_PROMPT_NODE_LIMIT+(!strcmp(mode,"too-many-nodes")?1U:0U));CHECK(owned);json=owned;
        if(!strcmp(mode,"too-many-nodes"))expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(!strcmp(mode,"max-bytes") || !strcmp(mode,"too-many-bytes")){
        size_t size=UMI_COMFY_PROMPT_MAX_BYTES+(!strcmp(mode,"too-many-bytes")?1U:0U);
        owned=malloc(size+1U);CHECK(owned);memset(owned,' ',size);memcpy(owned,workflow,sizeof(workflow)-1);owned[size]=0;json=owned;
        if(!strcmp(mode,"too-many-bytes"))expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(!strcmp(mode,"ownership")){
        owned=malloc(sizeof(workflow));CHECK(owned);memcpy(owned,workflow,sizeof(workflow));json=owned;
    }else if(!strcmp(mode,"cancel")){
        CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);umi_cancellation_token_request(cancel);expected=UMI_STATUS_CANCELLED;
    }
    if(json){d.workflow_json=json;d.workflow_length=strlen(json);}
    if(!strcmp(mode,"embedded-nul")){d.workflow_length=sizeof(workflow);expected=UMI_STATUS_PARSE_ERROR;}
    if(!strcmp(mode,"null-draft")){CHECK(UmiComfyPromptCreate(&c,revision,NULL,NULL,&prompt)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
    if(!strcmp(mode,"null-output")){CHECK(UmiComfyPromptCreate(&c,revision,&d,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
    if(!strcmp(mode,"null-connection")){CHECK(UmiComfyPromptCreate(NULL,revision,&d,NULL,&prompt)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
    UmiStatus status=UmiComfyPromptCreate(&c,revision,&d,cancel,&prompt);
    CHECK(status==expected);umi_cancellation_token_destroy(cancel);
    if(status!=UMI_STATUS_OK){CHECK(prompt==NULL);free(owned);return 0;}
    UmiComfyPromptReview review;const char *body=NULL;
    CHECK(UmiComfyPromptInspect(prompt,&review,&body)==UMI_STATUS_OK);
    CHECK(review.connection_revision==revision && review.body_bytes==strlen(body));
    CHECK(!strcmp(review.url,"http://127.0.0.1:8188/prompt"));
    char expected_hex[65],actual_hex[65];
    CHECK(UmiSha256Buffer(body,review.body_bytes,expected_hex)==UMI_STATUS_OK);
    UmiSha256Hex(review.request_sha256,actual_hex);CHECK(!strcmp(expected_hex,actual_hex));
    UmiJsonTree *tree=NULL;CHECK(UmiJsonTreeCreate(body,review.body_bytes,NULL,NULL,&tree)==UMI_STATUS_OK);
    int node=-1;char client[129];
    CHECK(UmiJsonTreeMember(tree,0,"client_id",&node)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeText(tree,node,client,sizeof(client))==UMI_STATUS_OK && !strcmp(client,d.client_id));
    CHECK(UmiJsonTreeMember(tree,0,"prompt",&node)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeKind(tree,node)==UMI_LANGUAGE_RUNTIME_JSON_OBJECT);
    CHECK(UmiJsonTreeCount(tree,node)==review.node_count);UmiJsonTreeDestroy(tree);
    if(!strcmp(mode,"body"))CHECK(!strcmp(body,"{\"client_id\":\"fixture-client\",\"prompt\":{\"node\":{\"class_type\":\"ExampleNode\",\"inputs\":{\"value\":7}}}}"));
    if(!strcmp(mode,"source-spelling"))CHECK(strstr(body,json)!=NULL);
    if(!strcmp(mode,"ownership")){memset(owned,'x',sizeof(workflow)-1);memset(d.client_id,0,sizeof(d.client_id));memset(&c,0,sizeof(c));CHECK(strstr(body,workflow)!=NULL && !strcmp(review.client_id,"fixture-client"));}
    if(!strcmp(mode,"occupied")){UmiComfyPrompt *same=prompt;CHECK(UmiComfyPromptCreate(&c,revision,&d,NULL,&prompt)==UMI_STATUS_INVALID_ARGUMENT && prompt==same);}
    if(!strcmp(mode,"review-refusal")){
        UmiComfyPromptReview before;memset(&review,0x5a,sizeof(review));before=review;
        const char *sentinel=body;
        CHECK(UmiComfyPromptInspect(NULL,&review,&body)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&review,&before,sizeof(review)) && body==sentinel);
        CHECK(UmiComfyPromptInspect(prompt,NULL,&body)==UMI_STATUS_INVALID_ARGUMENT && body==sentinel);
        CHECK(UmiComfyPromptInspect(prompt,&review,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }
    if(!strcmp(mode,"same-connection"))CHECK(UmiComfyPromptCheckConnection(prompt,&c,revision)==UMI_STATUS_OK);
    if(!strcmp(mode,"changed-label")){strcpy(c.label,"Different label");CHECK(UmiComfyPromptCheckConnection(prompt,&c,revision)==UMI_STATUS_BUSY);}
    if(!strcmp(mode,"changed-timeout")){c.timeout_ms++;CHECK(UmiComfyPromptCheckConnection(prompt,&c,revision)==UMI_STATUS_BUSY);}
    if(!strcmp(mode,"changed-endpoint")){strcpy(c.endpoint,"http://127.0.0.1:8189");CHECK(UmiComfyPromptCheckConnection(prompt,&c,revision)==UMI_STATUS_BUSY);}
    if(!strcmp(mode,"changed-revision"))CHECK(UmiComfyPromptCheckConnection(prompt,&c,revision+1)==UMI_STATUS_BUSY);
    if(!strncmp(mode,"task-",5) || !strcmp(mode,"prepare-task") || !strcmp(mode,"confirm-history")){
        UmiProviderTaskReceipt task,before;memset(&task,0x5a,sizeof(task));before=task;
        char long_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY+1];memset(long_id,'a',sizeof(long_id));long_id[sizeof(long_id)-1]=0;
        const char *app="media",*profile="local",*id="example";UmiStatus wanted=UMI_STATUS_OK;
        if(!strcmp(mode,"task-empty-scope")){profile="";wanted=UMI_STATUS_INVALID_ARGUMENT;}
        if(!strcmp(mode,"task-long-scope")){id=long_id;wanted=UMI_STATUS_CAPACITY_EXCEEDED;}
        if(!strcmp(mode,"task-invalid-scope")){app="Not-Valid";wanted=UMI_STATUS_INVALID_ARGUMENT;}
        if(!strcmp(mode,"task-null-scope")){profile=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT;}
        if(!strcmp(mode,"task-null-output")){CHECK(UmiComfyPromptPrepareTask(prompt,app,profile,id,NULL)==UMI_STATUS_INVALID_ARGUMENT);}
        else {
            CHECK(UmiComfyPromptPrepareTask(prompt,app,profile,id,&task)==wanted);
            if(wanted!=UMI_STATUS_OK)CHECK(!memcmp(&task,&before,sizeof(task)));
            else {
                CHECK(task.phase==UMI_PROVIDER_TASK_PREPARED && task.revision==1 && task.connection_revision==revision);
                CHECK(!memcmp(task.context.request_sha256,review.request_sha256,UMI_SHA256_BYTES));
                CHECK(!memcmp(task.connection_fingerprint,review.connection_sha256,UMI_SHA256_BYTES));
                CHECK(!strcmp(task.context.operation_id,UMI_COMFY_TASK_OPERATION));
                if(!strcmp(mode,"confirm-history")){
                    const char reply[]="{\"prompt_id\":\"job-1\",\"number\":4,\"node_errors\":{}}";
                    const char history[]="{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{}}}";
                    CHECK(UmiProviderTaskAdvance(&task,task.revision,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&task)==UMI_STATUS_OK);
                    CHECK(UmiComfyTaskConfirmJson(&task,task.revision,200,reply,sizeof(reply)-1,NULL,&task)==UMI_STATUS_OK);
                    UmiComfyHistory *h=NULL;CHECK(UmiComfyHistoryCreate(history,sizeof(history)-1,"job-1",NULL,&h)==UMI_STATUS_OK);
                    CHECK(UmiComfyTaskObserveHistory(&task,task.revision,h,&task)==UMI_STATUS_OK && task.phase==UMI_PROVIDER_TASK_SUCCEEDED);
                    CHECK(task.output_bytes==0);UmiComfyHistoryDestroy(h);
                }
            }
        }
    }
    UmiComfyPromptDestroy(prompt);UmiComfyPromptDestroy(NULL);free(owned);return 0;
}
