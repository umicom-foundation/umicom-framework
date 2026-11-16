/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_submission.c
 *
 * PURPOSE:
 *   Exercise accepted, partial and rejected submission observations and durable task confirmation boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "comfy_recovery_fixture.h"
static const char accepted[]="{\"prompt_id\":\"job-1\",\"number\":1,\"node_errors\":{}}";
static int task_case(const char *mode)
{
    UmiProviderTaskReceipt receipt,out,before;
    CHECK(recovery_task_fixture(UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN,&receipt)==0);
    const char *json=accepted; uint64_t revision=receipt.revision; UmiStatus expected=UMI_STATUS_OK;
    if(strcmp(mode,"task-confirm")==0) {
        CHECK(UmiComfyTaskConfirmJson(&receipt,revision,200,json,strlen(json),NULL,&receipt)==UMI_STATUS_OK);
        CHECK(receipt.phase==UMI_PROVIDER_TASK_SUBMITTED && receipt.revision==revision+1);
        CHECK(strcmp(receipt.remote_id,"job-1")==0 && receipt.output_bytes==0); return 0;
    }
    if(strcmp(mode,"task-repeat")==0) {
        CHECK(UmiComfyTaskConfirmJson(&receipt,revision,200,json,strlen(json),NULL,&receipt)==UMI_STATUS_OK);
        revision=receipt.revision;
        CHECK(UmiComfyTaskConfirmJson(&receipt,revision,200,json,strlen(json),NULL,&receipt)==UMI_STATUS_OK);
        CHECK(receipt.revision==revision && receipt.phase==UMI_PROVIDER_TASK_SUBMITTED); return 0;
    }
    if(strcmp(mode,"task-stale")==0) { --revision; expected=UMI_STATUS_BUSY; }
    else if(strcmp(mode,"task-provider")==0) { strcpy(receipt.provider_id,"other"); expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"task-operation")==0) { strcpy(receipt.context.operation_id,"other"); expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"task-not-begun")==0) { CHECK(recovery_task_fixture(UMI_PROVIDER_TASK_PREPARED,&receipt)==0); revision=receipt.revision; expected=UMI_STATUS_INVALID_STATE; }
    else if(strcmp(mode,"task-different")==0 || strcmp(mode,"task-running")==0) {
        CHECK(UmiProviderTaskConfirm(&receipt,receipt.revision,"other",&receipt)==UMI_STATUS_OK);
        if(strcmp(mode,"task-running")==0) CHECK(UmiProviderTaskAdvance(&receipt,receipt.revision,UMI_PROVIDER_TASK_OBSERVE_RUNNING,&receipt)==UMI_STATUS_OK);
        revision=receipt.revision; expected=UMI_STATUS_INVALID_STATE;
    } else if(strcmp(mode,"task-invalid-id")==0) { json="{\"prompt_id\":\"../job\",\"number\":0,\"node_errors\":{}}"; expected=UMI_STATUS_INVALID_ARGUMENT; }
    else CHECK(0);
    memset(&out,0xa5,sizeof(out)); memcpy(&before,&out,sizeof(out));
    CHECK(UmiComfyTaskConfirmJson(&receipt,revision,200,json,strlen(json),NULL,&out)==expected);
    CHECK(memcmp(&before,&out,sizeof(out))==0); return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2); const char *mode=argv[1];
    if(strncmp(mode,"task-",5)==0) return task_case(mode);
    const char *json=accepted; char generated[600]; unsigned http=200;
    UmiStatus expected=UMI_STATUS_OK; UmiCancellationToken *cancel=NULL;
    if(strcmp(mode,"accepted")==0) {}
    else if(strcmp(mode,"fractional")==0) json="{\"prompt_id\":\"job-1\",\"number\":1.5,\"node_errors\":{}}";
    else if(strcmp(mode,"negative")==0) json="{\"prompt_id\":\"job-1\",\"number\":-2,\"node_errors\":{}}";
    else if(strcmp(mode,"partial")==0) json="{\"prompt_id\":\"job-1\",\"number\":1,\"node_errors\":{\"unused\":{\"errors\":[\"synthetic\"]}}}";
    else if(strcmp(mode,"null-error")==0) json="{\"prompt_id\":\"job-1\",\"number\":1,\"node_errors\":{},\"error\":null}";
    else if(strcmp(mode,"unknown")==0) json="{\"prompt_id\":\"job-1\",\"number\":1,\"node_errors\":{},\"extra\":[true,null]}";
    else if(strcmp(mode,"missing-id")==0) { json="{\"number\":1,\"node_errors\":{}}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"empty-id")==0) { json="{\"prompt_id\":\"\",\"number\":1,\"node_errors\":{}}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"long-id")==0) { char id[130]; memset(id,'x',129); id[129]=0; snprintf(generated,sizeof(generated),"{\"prompt_id\":\"%s\",\"number\":1,\"node_errors\":{}}",id); json=generated; expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(strcmp(mode,"missing-number")==0) { json="{\"prompt_id\":\"job-1\",\"node_errors\":{}}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"wrong-number")==0) { json="{\"prompt_id\":\"job-1\",\"number\":\"1\",\"node_errors\":{}}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"boolean-number")==0) { json="{\"prompt_id\":\"job-1\",\"number\":true,\"node_errors\":{}}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"null-number")==0) { json="{\"prompt_id\":\"job-1\",\"number\":null,\"node_errors\":{}}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"max-number")==0 || strcmp(mode,"long-number")==0) {
        char n[66]; size_t length=strcmp(mode,"max-number")==0?64U:65U;
        memset(n,'1',length); n[length]=0;
        snprintf(generated,sizeof(generated),"{\"prompt_id\":\"job-1\",\"number\":%s,\"node_errors\":{}}",n); json=generated;
        if(length==65U) expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if(strcmp(mode,"missing-node-errors")==0) { json="{\"prompt_id\":\"job-1\",\"number\":1}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"wrong-node-errors")==0) { json="{\"prompt_id\":\"job-1\",\"number\":1,\"node_errors\":[]}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"error")==0) { json="{\"prompt_id\":\"job-1\",\"number\":1,\"node_errors\":{},\"error\":{}}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"duplicate")==0) { json="{\"prompt_id\":\"job-1\",\"number\":1,\"node_errors\":{},\"number\":2}"; expected=UMI_STATUS_ALREADY_EXISTS; }
    else if(strcmp(mode,"escaped-duplicate")==0) { json="{\"prompt_id\":\"job-1\",\"number\":1,\"node_errors\":{\"x\":1,\"\\u0078\":2}}"; expected=UMI_STATUS_ALREADY_EXISTS; }
    else if(strcmp(mode,"trailing")==0) { snprintf(generated,sizeof(generated),"%s{}",accepted); json=generated; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"embedded-nul")==0) { memcpy(generated,accepted,sizeof(accepted)); generated[2]=0; json=generated; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"root")==0) { json="[]"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"http")==0) { http=500; expected=UMI_STATUS_UNAVAILABLE; }
    else if(strcmp(mode,"cancel")==0) { CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK); umi_cancellation_token_request(cancel); expected=UMI_STATUS_CANCELLED; }
    else if(strcmp(mode,"nulls")==0) { UmiComfySubmission out; CHECK(UmiComfySubmissionDecode(200,accepted,strlen(accepted),NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT); CHECK(UmiComfySubmissionDecode(200,NULL,0,NULL,&out)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
    else CHECK(0);
    UmiComfySubmission out,before; memset(&out,0xa5,sizeof(out)); memcpy(&before,&out,sizeof(out));
    size_t length=strcmp(mode,"embedded-nul")==0?sizeof(accepted)-1:strlen(json);
    CHECK(UmiComfySubmissionDecode(http,json,length,cancel,&out)==expected);
    if(expected!=UMI_STATUS_OK) CHECK(memcmp(&out,&before,sizeof(out))==0);
    else {
        CHECK(strcmp(out.prompt_id,"job-1")==0);
        CHECK(out.node_error_count==(strcmp(mode,"partial")==0?1U:0U));
        if(strcmp(mode,"fractional")==0) CHECK(strcmp(out.queue_number,"1.5")==0);
        if(strcmp(mode,"negative")==0) CHECK(strcmp(out.queue_number,"-2")==0);
        if(strcmp(mode,"max-number")==0) CHECK(strlen(out.queue_number)==64U);
    }
    umi_cancellation_token_destroy(cancel); return 0;
}
