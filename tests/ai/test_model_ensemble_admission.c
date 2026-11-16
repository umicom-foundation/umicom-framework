/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_model_ensemble_admission.c
 *
 * PURPOSE:
 *   Exercise bounded multi-model report selection, shared response admission and signed UI limits without network effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/model_ensemble.h"
#include "umicom/ai_ui/model_comparison_views.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while (0)
static int Fill(UmiAiModelEnsembleReport *report)
{
    CHECK(umi_ai_model_ensemble_report_initialize(report)==UMI_STATUS_OK);
    memcpy(report->request_id,"comparison",11U);report->result_count=2U;report->success_count=1U;report->selected_index=0U;report->revision=1U;
    for(size_t i=0U;i<2U;++i){
        UmiAiModelResult *result=&report->results[i];
        result->structure_size=(uint32_t)sizeof *result;result->api_version=UMI_AI_MODEL_ENSEMBLE_API_VERSION;
        CHECK(umi_ai_model_target_initialize(&result->target,"fixture",i==0U?"model-a":"model-b","Fixture",0)==UMI_STATUS_OK);
        umi_ai_response_init(&result->response);
        result->status=i==0U?UMI_STATUS_OK:UMI_STATUS_UNAVAILABLE;
        if(i==0U)CHECK(umi_ai_response_set_text(&result->response,"Observed response")==UMI_STATUS_OK);
    }
    return 0;
}
static int Refused(UmiAiModelEnsembleReport *report)
{
    unsigned char *before=malloc(sizeof *report);CHECK(before!=NULL);memcpy(before,report,sizeof *report);
    CHECK(UmiAiModelEnsembleReportValidate(report)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ai_model_ensemble_selected(report)==NULL);
    CHECK(umi_ai_model_ensemble_select(report,0U)==UMI_STATUS_INVALID_ARGUMENT);
    /* The invalid result count must not let an oversized selected index form
     * an address outside the eight-result array, even before dereferencing. */
    CHECK(umi_ai_model_ensemble_select(report,UMI_AI_MODEL_ENSEMBLE_MAX_TARGETS)==UMI_STATUS_INVALID_ARGUMENT);
    UmiUiViewModel *sentinel=(UmiUiViewModel *)(uintptr_t)1U;
    CHECK(umi_ai_ui_model_comparison_view_create("comparison",report,&sentinel)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(sentinel==(UmiUiViewModel *)(uintptr_t)1U && !memcmp(before,report,sizeof *report));free(before);return 0;
}
static size_t calls;
static UmiStatus Generate(void *instance,const UmiAiRequest *request,UmiAiResponse *response)
{
    (void)request;++calls;umi_ai_response_init(response);
    if(*(int *)instance)response->finish_reason=(UmiAiFinishReason)99;
    return umi_ai_response_set_text(response,"Synthetic provider response");
}
static int Query(const char *mode)
{
    UmiAiRuntime runtime;umi_ai_runtime_init(&runtime);
    int malformed=1,valid=0;
    UmiAiProvider provider={0};provider.structure_size=(uint32_t)sizeof provider;provider.abi_version=1U;
    provider.provider_id="bad";provider.kind=UMI_AI_PROVIDER_TEST;provider.instance=&malformed;provider.generate=Generate;
    CHECK(umi_ai_provider_registry_add(&runtime.providers,&provider)==UMI_STATUS_OK);
    provider.provider_id="good";provider.instance=&valid;
    CHECK(umi_ai_provider_registry_add(&runtime.providers,&provider)==UMI_STATUS_OK);
    UmiAiModelTarget targets[2];
    CHECK(umi_ai_model_target_initialize(&targets[0],"bad","model","Bad",0)==UMI_STATUS_OK);
    CHECK(umi_ai_model_target_initialize(&targets[1],"good","model","Good",0)==UMI_STATUS_OK);
    UmiAiRequest request;umi_ai_request_init(&request);
    UmiAiModelEnsembleReport *report=malloc(sizeof *report);CHECK(report!=NULL);
    unsigned char *before=malloc(sizeof *report);CHECK(before!=NULL);
    memset(report,0xa5,sizeof *report);memcpy(before,report,sizeof *report);calls=0U;
    if(!strcmp(mode,"query-target-flags")){
        targets[1].approved=2;
        CHECK(umi_ai_model_ensemble_query(&runtime,&request,targets,2U,report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(calls==0U && !memcmp(before,report,sizeof *report));
        targets[1].approved=0;targets[1].enabled=-1;
        CHECK(umi_ai_model_ensemble_query(&runtime,&request,targets,2U,report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(calls==0U && !memcmp(before,report,sizeof *report));
    }else if(!strcmp(mode,"query-request-count")){
        request.message_count=UMI_AI_MAX_MESSAGES+1U;
        CHECK(umi_ai_model_ensemble_query(&runtime,&request,targets,2U,report)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(calls==0U && !memcmp(before,report,sizeof *report));
    }else if(!strcmp(mode,"query-finish")){
        CHECK(umi_ai_model_ensemble_query(&runtime,&request,targets,1U,report)==UMI_STATUS_INTERNAL_ERROR);
        CHECK(calls==1U && report->success_count==0U && report->selected_index==SIZE_MAX);
        CHECK(report->results[0].status==UMI_STATUS_INTERNAL_ERROR && UmiAiModelEnsembleReportValidate(report)==UMI_STATUS_OK);
    }else{
        CHECK(umi_ai_model_ensemble_query(&runtime,&request,targets,2U,report)==UMI_STATUS_OK);
        CHECK(calls==2U && report->success_count==1U && report->selected_index==1U);
        CHECK(report->results[0].status==UMI_STATUS_INTERNAL_ERROR && report->results[1].status==UMI_STATUS_OK);
        CHECK(UmiAiModelEnsembleReportValidate(report)==UMI_STATUS_OK);
    }
    free(before);free(report);CHECK(UmiAiRuntimeRelease(&runtime)==UMI_STATUS_OK);return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    if(!strncmp(mode,"query-",6U))return Query(mode);
    UmiAiModelEnsembleReport report;CHECK(Fill(&report)==0);
    if(!strcmp(mode,"empty")){
        CHECK(umi_ai_model_ensemble_report_initialize(&report)==UMI_STATUS_OK);
        CHECK(UmiAiModelEnsembleReportValidate(&report)==UMI_STATUS_OK && umi_ai_model_ensemble_selected(&report)==NULL);return 0;
    }
    if(!strcmp(mode,"valid"))CHECK(UmiAiModelEnsembleReportValidate(&report)==UMI_STATUS_OK);
    else if(!strcmp(mode,"maximum")){
        for(size_t i=2U;i<UMI_AI_MODEL_ENSEMBLE_MAX_TARGETS;++i)report.results[i]=report.results[1];
        report.result_count=UMI_AI_MODEL_ENSEMBLE_MAX_TARGETS;
        CHECK(UmiAiModelEnsembleReportValidate(&report)==UMI_STATUS_OK);
    }else if(!strcmp(mode,"failed-response")){
        memset(&report.results[1].response,0xff,sizeof report.results[1].response);
        CHECK(UmiAiModelEnsembleReportValidate(&report)==UMI_STATUS_OK);
    }else if(!strcmp(mode,"inactive-slots")){
        memset(&report.results[2],0xff,sizeof report.results[2]);CHECK(UmiAiModelEnsembleReportValidate(&report)==UMI_STATUS_OK);
    }else if(!strcmp(mode,"select-success")){
        report.results[1].status=UMI_STATUS_OK;report.success_count=2U;
        CHECK(umi_ai_model_ensemble_select(&report,1U)==UMI_STATUS_OK && report.selected_index==1U && report.revision==2U);
    }else if(!strcmp(mode,"select-failed")||!strcmp(mode,"select-overflow")){
        if(!strcmp(mode,"select-overflow"))report.revision=UINT64_MAX;
        unsigned char before[sizeof report];memcpy(before,&report,sizeof report);
        CHECK(umi_ai_model_ensemble_select(&report,!strcmp(mode,"select-overflow")?0U:1U)==
            (!strcmp(mode,"select-overflow")?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!memcmp(before,&report,sizeof report));
    }else if(!strcmp(mode,"borrowed-result")){
        CHECK(umi_ai_model_ensemble_selected(&report)==&report.results[0]);report.selected_index=SIZE_MAX;
        CHECK(UmiAiModelEnsembleReportValidate(&report)==UMI_STATUS_OK && umi_ai_model_ensemble_selected(&report)==NULL);
    }else if(!strcmp(mode,"nulls")){
        CHECK(UmiAiModelEnsembleReportValidate(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_model_ensemble_select(NULL,0U)==UMI_STATUS_INVALID_ARGUMENT && umi_ai_model_ensemble_selected(NULL)==NULL);
    }else if(!strncmp(mode,"view-",5U)){
        UmiUiViewModel *view=NULL;UmiUiValue value;
        if(!strcmp(mode,"view-revision-max"))report.revision=INT64_MAX;
        if(!strcmp(mode,"view-revision-overflow")){
            report.revision=(uint64_t)INT64_MAX+1U;
            CHECK(umi_ai_ui_model_comparison_view_create("comparison",&report,&view)==UMI_STATUS_CAPACITY_EXCEEDED && view==NULL);
        }else{
            CHECK(umi_ai_ui_model_comparison_view_create("comparison",&report,&view)==UMI_STATUS_OK);
            CHECK(umi_ui_view_model_get_property(view,"ai-models.revision",&value)==UMI_STATUS_OK && value.integer_value==(int64_t)report.revision);
            umi_ui_view_model_destroy(view);
        }
    }else{
        if(!strcmp(mode,"oversized-count")){report.result_count=9U;report.selected_index=8U;}
        else if(!strcmp(mode,"max-count")){report.result_count=SIZE_MAX;report.selected_index=SIZE_MAX-1U;}
        else if(!strcmp(mode,"selected-outside"))report.selected_index=2U;
        else if(!strcmp(mode,"selected-failed"))report.selected_index=1U;
        else if(!strcmp(mode,"success-count"))report.success_count=2U;
        else if(!strcmp(mode,"result-size"))report.results[0].structure_size=1U;
        else if(!strcmp(mode,"result-version"))report.results[0].api_version=99U;
        else if(!strcmp(mode,"target-size"))report.results[0].target.structure_size=1U;
        else if(!strcmp(mode,"target-version"))report.results[0].target.api_version=99U;
        else if(!strcmp(mode,"target-enabled"))report.results[0].target.enabled=2;
        else if(!strcmp(mode,"target-approved"))report.results[0].target.approved=-1;
        else if(!strcmp(mode,"target-provider"))memset(report.results[0].target.provider_id,'x',sizeof report.results[0].target.provider_id);
        else if(!strcmp(mode,"target-model"))report.results[0].target.model_id[0]='\0';
        else if(!strcmp(mode,"target-label"))memset(report.results[0].target.label,'x',sizeof report.results[0].target.label);
        else if(!strcmp(mode,"request-text"))memset(report.request_id,'x',sizeof report.request_id);
        else if(!strcmp(mode,"response-text"))memset(report.results[0].response.text,'x',sizeof report.results[0].response.text);
        else if(!strcmp(mode,"response-identity"))memset(report.results[0].response.model_id,'x',sizeof report.results[0].response.model_id);
        else if(!strcmp(mode,"response-finish"))report.results[0].response.finish_reason=(UmiAiFinishReason)99;
        else CHECK(0);
        return Refused(&report);
    }
    return 0;
}
