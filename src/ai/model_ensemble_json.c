/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/model_ensemble_json.c
 *
 * PURPOSE:
 *   Project admitted comparison metadata through the shared JSON writer without publishing partial output or response text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/model_ensemble_json.h"
#include "umicom/protocol/json_writer.h"
#include "umicom/security/secrets.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void ReceiptKey(UmiJsonWriter *writer, const char *key)
{
    UmiJsonWriterRaw(writer,",");
    UmiJsonWriterString(writer,key);
    UmiJsonWriterRaw(writer,":");
}
static void ReceiptText(UmiJsonWriter *writer,const char *key,const char *value)
{
    ReceiptKey(writer,key);
    UmiJsonWriterString(writer,value);
}
UmiStatus UmiAiModelEnsembleReportJson(const UmiAiModelEnsembleReport *report,
    char *out,size_t capacity,size_t *out_length)
{
    if(out==NULL || out_length==NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=UmiAiModelEnsembleReportValidate(report);
    if(status!=UMI_STATUS_OK)
        return status;
    if(capacity==0U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t bound=capacity<UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY?
        capacity:UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY;
    char *staged=malloc(bound);
    if(staged==NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer,staged,bound);
    UmiJsonWriterRaw(&writer,"{\"schema_version\":1,\"scope\":\"captured-model-comparison\"");
    ReceiptText(&writer,"request_id",report->request_id);
    /* Decimal text avoids losing revision bits in a JavaScript JSON consumer. */
    char revision[32];
    int written=snprintf(revision,sizeof revision,"%" PRIu64,report->revision);
    if(written<0 || (size_t)written>=sizeof revision)
        writer.status=UMI_STATUS_INTERNAL_ERROR;
    else
        ReceiptText(&writer,"revision",revision);
    ReceiptKey(&writer,"result_count");
    UmiJsonWriterUInt64(&writer,(uint64_t)report->result_count);
    ReceiptKey(&writer,"success_count");
    UmiJsonWriterUInt64(&writer,(uint64_t)report->success_count);
    ReceiptKey(&writer,"selected_index");
    if(report->selected_index==SIZE_MAX)
        UmiJsonWriterRaw(&writer,"null");
    else
        UmiJsonWriterUInt64(&writer,(uint64_t)report->selected_index);
    ReceiptKey(&writer,"results");
    UmiJsonWriterRaw(&writer,"[");
    for(size_t index=0U;index<report->result_count;++index)
    {
        const UmiAiModelResult *result=&report->results[index];
        if(index!=0U) UmiJsonWriterRaw(&writer,",");
        UmiJsonWriterRaw(&writer,"{\"index\":");
        UmiJsonWriterUInt64(&writer,(uint64_t)index);
        ReceiptKey(&writer,"status_code");
        UmiJsonWriterInt64(&writer,(int64_t)result->status);
        ReceiptKey(&writer,"selected");
        UmiJsonWriterBool(&writer,index==report->selected_index);
        ReceiptKey(&writer,"target");
        UmiJsonWriterRaw(&writer,"{\"provider_id\":");
        UmiJsonWriterString(&writer,result->target.provider_id);
        ReceiptText(&writer,"model_id",result->target.model_id);
        ReceiptText(&writer,"label",result->target.label);
        ReceiptKey(&writer,"enabled");
        UmiJsonWriterBool(&writer,result->target.enabled);
        ReceiptKey(&writer,"approved");
        UmiJsonWriterBool(&writer,result->target.approved);
        UmiJsonWriterRaw(&writer,"}");
        ReceiptKey(&writer,"response");
        if(result->status!=UMI_STATUS_OK)
        {
            /* Failed provider storage may be partial or unterminated. Its
             * status remains evidence; no failed response field is inspected. */
            UmiJsonWriterRaw(&writer,"null");
        }
        else
        {
            UmiJsonWriterRaw(&writer,"{\"request_id\":");
            UmiJsonWriterString(&writer,result->response.request_id);
            ReceiptText(&writer,"provider_id",result->response.provider_id);
            ReceiptText(&writer,"model_id",result->response.model_id);
            ReceiptKey(&writer,"finish_reason");
            UmiJsonWriterInt64(&writer,(int64_t)result->response.finish_reason);
            UmiJsonWriterRaw(&writer,"}");
        }
        UmiJsonWriterRaw(&writer,"}");
    }
    UmiJsonWriterRaw(&writer,"]}");
    status=writer.status;
    /* Every fallible validation/formatting operation finishes before publication. */
    if(status==UMI_STATUS_OK)
    {
        memcpy(out,staged,writer.length+1U);
        *out_length=writer.length;
    }
    umi_secret_clear(staged,bound);
    free(staged);
    return status;
}
