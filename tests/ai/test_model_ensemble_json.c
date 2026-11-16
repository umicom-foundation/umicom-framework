/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_model_ensemble_json.c
 *
 * PURPOSE:
 *   Verify exact comparison receipts, omitted private content, failure isolation and unchanged caller output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/model_ensemble_json.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); failed=1;goto cleanup; } } while (0)
static int Member(const UmiJsonTree *tree,int object,const char *name)
{
    int value=-1;
    return UmiJsonTreeMember(tree,object,name,&value)==UMI_STATUS_OK?value:-1;
}
static int TextIs(const UmiJsonTree *tree,int node,const char *expected)
{
    char value[1024];
    return UmiJsonTreeText(tree,node,value,sizeof value)==UMI_STATUS_OK && !strcmp(value,expected);
}
static int IntegerIs(const UmiJsonTree *tree,int node,int64_t expected)
{
    int64_t value=-1;
    return UmiJsonTreeInteger(tree,node,&value)==UMI_STATUS_OK && value==expected;
}
static int BooleanIs(const UmiJsonTree *tree,int node,int expected)
{
    int value=-1;
    return UmiJsonTreeBoolean(tree,node,&value)==UMI_STATUS_OK && value==expected;
}
static UmiStatus Fixture(UmiAiModelEnsembleReport *report,size_t count)
{
    UmiStatus status=umi_ai_model_ensemble_report_initialize(report);
    if(status!=UMI_STATUS_OK)return status;
    strcpy(report->request_id,"request");
    report->result_count=count;
    report->success_count=count?1U:0U;
    report->selected_index=count?0U:SIZE_MAX;
    report->revision=7U;
    for(size_t i=0U;i<count;++i){
        UmiAiModelResult *r=&report->results[i];
        r->structure_size=(uint32_t)sizeof *r;r->api_version=UMI_AI_MODEL_ENSEMBLE_API_VERSION;
        status=umi_ai_model_target_initialize(&r->target,"requested-provider","requested-model","Target",1);
        if(status!=UMI_STATUS_OK)return status;
        r->status=i?UMI_STATUS_UNAVAILABLE:UMI_STATUS_OK;
        umi_ai_response_init(&r->response);
        strcpy(r->response.request_id,"request");
        strcpy(r->response.provider_id,"requested-provider");
        strcpy(r->response.model_id,"requested-model");
        strcpy(r->response.text,"PRIVATE_ANSWER_SENTINEL");
        r->response.finish_reason=UMI_AI_FINISH_STOP;
        r->response.usage.input_tokens=0U;
        r->response.usage.output_tokens=0U;
        r->response.usage.total_tokens=0U;
    }
    return UMI_STATUS_OK;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    int failed=0;
    const char *mode=argv[1];
    UmiAiModelEnsembleReport *report=calloc(1U,sizeof *report),*before=malloc(sizeof *before);
    char *output=malloc(UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY),*old=malloc(UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY);
    UmiJsonTree *tree=NULL;
    CHECK(report && before && output && old);
    CHECK(Fixture(report,!strcmp(mode,"empty")?0U:2U)==UMI_STATUS_OK);
    UmiStatus wanted=UMI_STATUS_OK;
    if(!strcmp(mode,"all-failed")){
        report->results[0].status=UMI_STATUS_TIMEOUT;report->success_count=0U;report->selected_index=SIZE_MAX;
    }else if(!strcmp(mode,"all-success") || !strcmp(mode,"second-selected")){
        report->results[1].status=UMI_STATUS_OK;report->success_count=2U;
        if(!strcmp(mode,"second-selected"))report->selected_index=1U;
    }else if(!strcmp(mode,"no-selection"))report->selected_index=SIZE_MAX;
    else if(!strcmp(mode,"revision-max"))report->revision=UINT64_MAX;
    else if(!strcmp(mode,"revision-adjacent"))report->revision=UINT64_C(9007199254740993);
    else if(!strcmp(mode,"unknown-status"))report->results[1].status=(UmiStatus)99;
    else if(!strcmp(mode,"identity-mismatch")){
        strcpy(report->results[0].response.request_id,"different-request");
        strcpy(report->results[0].response.provider_id,"reported-provider");
        strcpy(report->results[0].response.model_id,"reported-model");
    }else if(!strcmp(mode,"omitted-answer")){
        memset(report->results[0].response.text,'Z',sizeof report->results[0].response.text-1U);
        report->results[0].response.text[sizeof report->results[0].response.text-1U]='\0';
    }else if(!strcmp(mode,"omitted-usage")){
        report->results[0].response.usage.input_tokens=UINT64_MAX;
        report->results[0].response.usage.output_tokens=UINT64_MAX;
        report->results[0].response.usage.total_tokens=UINT64_MAX;
    }else if(!strcmp(mode,"empty-identity")){
        report->results[0].response.request_id[0]='\0';report->results[0].response.provider_id[0]='\0';
        report->results[0].response.model_id[0]='\0';
    }else if(!strcmp(mode,"unicode"))strcpy(report->results[0].target.label,"\xce\xbb \xf0\x9f\x9a\x80");
    else if(!strcmp(mode,"escaping"))strcpy(report->results[0].target.label,"quote\" slash\\ line\n");
    else if(!strcmp(mode,"control-label"))strcpy(report->results[0].target.label,"\1\2\t\r");
    else if(!strcmp(mode,"failed-garbage"))memset(&report->results[1].response,0xa5,sizeof report->results[1].response);
    else if(!strcmp(mode,"omitted-invalid-utf8")){report->results[0].response.text[0]=(char)0xff;report->results[0].response.text[1]='\0';}
    else if(!strcmp(mode,"max-fields")){
        CHECK(Fixture(report,UMI_AI_MODEL_ENSEMBLE_MAX_TARGETS)==UMI_STATUS_OK);
        report->success_count=report->result_count;
        for(size_t i=0U;i<report->result_count;++i){
            UmiAiModelResult *r=&report->results[i];r->status=UMI_STATUS_OK;
            memset(r->target.label,1,sizeof r->target.label-1U);r->target.label[sizeof r->target.label-1U]='\0';
            memset(r->target.provider_id,1,sizeof r->target.provider_id-1U);r->target.provider_id[sizeof r->target.provider_id-1U]='\0';
            memset(r->target.model_id,1,sizeof r->target.model_id-1U);r->target.model_id[sizeof r->target.model_id-1U]='\0';
            memset(r->response.request_id,1,sizeof r->response.request_id-1U);r->response.request_id[sizeof r->response.request_id-1U]='\0';
            memset(r->response.provider_id,1,sizeof r->response.provider_id-1U);r->response.provider_id[sizeof r->response.provider_id-1U]='\0';
            memset(r->response.model_id,1,sizeof r->response.model_id-1U);r->response.model_id[sizeof r->response.model_id-1U]='\0';
        }
    }else if(!strcmp(mode,"invalid-count")){report->result_count=9U;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"invalid-success-count")){report->success_count=2U;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"invalid-selection")){report->selected_index=3U;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"failed-selection")){report->selected_index=1U;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"invalid-report-size")){report->structure_size=1U;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"invalid-target")){report->results[1].target.enabled=2;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"invalid-response")){memset(report->results[0].response.text,'x',sizeof report->results[0].response.text);wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"utf8-request")){report->request_id[0]=(char)0xff;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"utf8-provider")){report->results[1].target.provider_id[0]=(char)0xff;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"utf8-label")){report->results[1].target.label[0]=(char)0xff;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"utf8-response")){report->results[0].response.model_id[0]=(char)0xff;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    size_t capacity=UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY,length=12345U;
    char *destination=output;
    size_t *lengthOutput=&length;
    const UmiAiModelEnsembleReport *input=report;
    if(!strcmp(mode,"exact-capacity") || !strcmp(mode,"small-capacity")){
        size_t needed=0U;
        CHECK(UmiAiModelEnsembleReportJson(report,output,capacity,&needed)==UMI_STATUS_OK);
        capacity=needed+(!strcmp(mode,"exact-capacity")?1U:0U);
        if(!strcmp(mode,"small-capacity"))wanted=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(!strcmp(mode,"zero-capacity")){capacity=0U;wanted=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(!strcmp(mode,"null-output")){destination=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"null-length")){lengthOutput=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"null-report")){input=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT;}
    memcpy(before,report,sizeof *report);
    memset(output,0x5a,UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY);
    memcpy(old,output,UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY);
    CHECK(UmiAiModelEnsembleReportJson(input,destination,capacity,lengthOutput)==wanted);
    CHECK(!memcmp(before,report,sizeof *report));
    if(wanted!=UMI_STATUS_OK){
        CHECK(length==12345U && !memcmp(old,output,UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY));
    }else{
        CHECK(length==strlen(output) && length<capacity);
        CHECK(!memcmp(output+length+1U,old+length+1U,UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY-length-1U));
        CHECK(!strstr(output,"PRIVATE_ANSWER_SENTINEL") && !strstr(output,"usage") && !strstr(output,"tokens"));
        CHECK(UmiJsonTreeCreate(output,length,NULL,NULL,&tree)==UMI_STATUS_OK);
        CHECK(UmiJsonTreeValidateUniqueKeys(tree,0,NULL)==UMI_STATUS_OK);
        CHECK(IntegerIs(tree,Member(tree,0,"schema_version"),1));
        CHECK(TextIs(tree,Member(tree,0,"scope"),"captured-model-comparison"));
        CHECK(TextIs(tree,Member(tree,0,"request_id"),report->request_id));
        const char *revision=report->revision==UINT64_MAX?"18446744073709551615":
            report->revision==UINT64_C(9007199254740993)?"9007199254740993":"7";
        CHECK(TextIs(tree,Member(tree,0,"revision"),revision));
        CHECK(IntegerIs(tree,Member(tree,0,"result_count"),(int64_t)report->result_count));
        CHECK(IntegerIs(tree,Member(tree,0,"success_count"),(int64_t)report->success_count));
        int selection=Member(tree,0,"selected_index");
        CHECK(report->selected_index==SIZE_MAX?UmiJsonTreeIsNull(tree,selection):
            IntegerIs(tree,selection,(int64_t)report->selected_index));
        int results=Member(tree,0,"results");
        CHECK(UmiJsonTreeCount(tree,results)==report->result_count);
        int row=UmiJsonTreeFirst(tree,results);
        for(size_t i=0U;i<report->result_count;++i){
            const UmiAiModelResult *r=&report->results[i];
            CHECK(IntegerIs(tree,Member(tree,row,"index"),(int64_t)i));
            CHECK(IntegerIs(tree,Member(tree,row,"status_code"),(int64_t)r->status));
            CHECK(BooleanIs(tree,Member(tree,row,"selected"),i==report->selected_index));
            int target=Member(tree,row,"target");
            CHECK(TextIs(tree,Member(tree,target,"provider_id"),r->target.provider_id));
            CHECK(TextIs(tree,Member(tree,target,"model_id"),r->target.model_id));
            CHECK(TextIs(tree,Member(tree,target,"label"),r->target.label));
            CHECK(BooleanIs(tree,Member(tree,target,"approved"),r->target.approved));
            CHECK(BooleanIs(tree,Member(tree,target,"enabled"),r->target.enabled));
            int response=Member(tree,row,"response");
            if(r->status!=UMI_STATUS_OK)CHECK(UmiJsonTreeIsNull(tree,response));
            else{
                CHECK(TextIs(tree,Member(tree,response,"request_id"),r->response.request_id));
                CHECK(TextIs(tree,Member(tree,response,"provider_id"),r->response.provider_id));
                CHECK(TextIs(tree,Member(tree,response,"model_id"),r->response.model_id));
                CHECK(IntegerIs(tree,Member(tree,response,"finish_reason"),(int64_t)r->response.finish_reason));
                CHECK(Member(tree,response,"text")==-1 && Member(tree,response,"usage")==-1);
            }
            row=UmiJsonTreeNext(tree,row);
        }
        if(!strcmp(mode,"copy-lifetime")){
            memset(report,0,sizeof *report);free(report);report=NULL;
            CHECK(strstr(output,"requested-model") && strstr(output,"captured-model-comparison"));
        }
    }
cleanup:
    UmiJsonTreeDestroy(tree);free(report);free(before);free(output);free(old);
    return failed;
}
