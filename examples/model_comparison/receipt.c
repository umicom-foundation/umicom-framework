/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/model_comparison/receipt.c
 *
 * PURPOSE:
 *   Export a synthetic multi-model metadata receipt through an installed Framework SDK without provider calls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/model_ensemble_json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    UmiAiModelEnsembleReport report;
    if(umi_ai_model_ensemble_report_initialize(&report)!=UMI_STATUS_OK)return 1;
    strcpy(report.request_id,"synthetic-comparison");
    report.result_count=2U;report.success_count=1U;report.selected_index=SIZE_MAX;
    report.revision=UINT64_C(9007199254740993);
    for(size_t i=0U;i<2U;++i){
        UmiAiModelResult *r=&report.results[i];
        r->structure_size=(uint32_t)sizeof *r;r->api_version=UMI_AI_MODEL_ENSEMBLE_API_VERSION;
        if(umi_ai_model_target_initialize(&r->target,"example-provider",
            i?"model-b":"model-a",i?"Unavailable capture":"Successful capture",0)!=UMI_STATUS_OK)return 1;
        r->status=i?UMI_STATUS_UNAVAILABLE:UMI_STATUS_OK;
        umi_ai_response_init(&r->response);
        if(i==0U){
            strcpy(r->response.request_id,report.request_id);
            strcpy(r->response.provider_id,"example-provider");
            strcpy(r->response.model_id,"model-a");
            strcpy(r->response.text,"This private example text must not enter the receipt.");
            r->response.finish_reason=UMI_AI_FINISH_STOP;
        }
    }
    char *json=malloc(UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY);
    if(!json)return 1;
    size_t bytes=0U;
    UmiStatus status=UmiAiModelEnsembleReportJson(&report,json,
        UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY,&bytes);
    /* A receipt captures current report metadata. It does not choose an answer
     * or make the synthetic identities evidence of a real provider response. */
    if(status!=UMI_STATUS_OK || report.selected_index!=SIZE_MAX ||
       !strstr(json,"\"revision\":\"9007199254740993\"") ||
       !strstr(json,"\"selected_index\":null") || !strstr(json,"\"response\":null") ||
       strstr(json,"private example text")){
        free(json);return 1;
    }
    int failed=fwrite(json,1U,bytes,stdout)!=bytes || fputc('\n',stdout)==EOF;
    free(json);
    return !failed && fflush(stdout)==0?0:1;
}
