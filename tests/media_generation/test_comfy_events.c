/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_events.c
 *
 * PURPOSE:
 *   Verify ComfyUI observation kinds, malformed messages and identity-bound task transitions without provider effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_events.h"
#include <stdio.h>
#include <string.h>
#define CHECK(e) do { if (!(e)) { fprintf(stderr,"%d: %s\n",__LINE__,#e); return 1; } } while(0)
static const char started[] = "{\"type\":\"execution_start\",\"data\":{\"prompt_id\":\"job-1\"}}";
static const char succeeded[] = "{\"type\":\"execution_success\",\"data\":{\"prompt_id\":\"job-1\",\"timestamp\":1}}";
static const char ui_output[] = "{\"type\":\"executed\",\"data\":{\"prompt_id\":\"job-1\",\"node\":\"1\",\"output\":{}}}";
static const char queue[] = "{\"type\":\"status\",\"data\":{\"exec_info\":{\"queue_remaining\":2}}}";
static int task_fixture(UmiProviderTaskReceipt *out)
{
    UmiProviderConnection c = {0};
    strcpy(c.id,"local-comfy"); strcpy(c.provider_id,"comfyui");
    strcpy(c.label,"Local ComfyUI"); strcpy(c.endpoint,"http://127.0.0.1:8188");
    strcpy(c.model,"workflow"); c.route=UMI_PROVIDER_CONNECTION_LOOPBACK; c.timeout_ms=30000; c.enabled=true;
    UmiProviderTaskContext context={0};
    strcpy(context.application_id,"media"); strcpy(context.profile_id,"owner");
    strcpy(context.local_id,"scene"); strcpy(context.operation_id,UMI_COMFY_TASK_OPERATION);
    context.request_sha256[0]=1;
    CHECK(UmiProviderTaskPrepare(&context,&c,1,out)==UMI_STATUS_OK);
    CHECK(UmiProviderTaskAdvance(out,out->revision,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,out)==UMI_STATUS_OK);
    CHECK(UmiProviderTaskConfirm(out,out->revision,"job-1",out)==UMI_STATUS_OK);
    return 0;
}
static int task_case(const char *mode)
{
    UmiProviderTaskReceipt receipt, out, before;
    CHECK(task_fixture(&receipt)==0);
    memset(&out,0xa5,sizeof(out)); memcpy(&before,&out,sizeof(out));
    const char *message=started;
    uint64_t revision=receipt.revision;
    UmiStatus expected=UMI_STATUS_OK;
    UmiCancellationToken *cancel=NULL;
    if(strcmp(mode,"task-lifecycle")==0) {
        CHECK(UmiComfyTaskObserveJson(&receipt,revision,started,sizeof(started)-1,NULL,&receipt)==UMI_STATUS_OK);
        CHECK(receipt.phase==UMI_PROVIDER_TASK_RUNNING && receipt.revision==revision+1);
        CHECK(UmiComfyTaskObserveJson(&receipt,receipt.revision,succeeded,sizeof(succeeded)-1,NULL,&receipt)==UMI_STATUS_OK);
        CHECK(receipt.phase==UMI_PROVIDER_TASK_SUCCEEDED && receipt.revision==revision+2);
        uint64_t terminal=receipt.revision;
        CHECK(UmiComfyTaskObserveJson(&receipt,terminal,succeeded,sizeof(succeeded)-1,NULL,&receipt)==UMI_STATUS_OK);
        CHECK(receipt.revision==terminal && receipt.output_bytes==0); return 0;
    } else if(strcmp(mode,"task-wrong-id")==0) { message="{\"type\":\"execution_success\",\"data\":{\"prompt_id\":\"other\"}}"; expected=UMI_STATUS_NOT_FOUND; }
    else if(strcmp(mode,"task-stale")==0) { --revision; expected=UMI_STATUS_BUSY; }
    else if(strcmp(mode,"task-provider")==0) { strcpy(receipt.provider_id,"different"); expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"task-operation")==0) { strcpy(receipt.context.operation_id,"different"); expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"task-unconfirmed")==0) { receipt.phase=UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN; receipt.remote_id[0]='\0'; expected=UMI_STATUS_INVALID_STATE; }
    else if(strcmp(mode,"task-ui")==0) { message=ui_output; expected=UMI_STATUS_UNAVAILABLE; }
    else if(strcmp(mode,"task-global")==0) { message=queue; expected=UMI_STATUS_UNAVAILABLE; }
    else if(strcmp(mode,"task-terminal")==0) { CHECK(UmiProviderTaskAdvance(&receipt,receipt.revision,UMI_PROVIDER_TASK_OBSERVE_SUCCESS,&receipt)==UMI_STATUS_OK); revision=receipt.revision; expected=UMI_STATUS_INVALID_STATE; }
    else if(strcmp(mode,"task-cancel")==0) { CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK); umi_cancellation_token_request(cancel); expected=UMI_STATUS_CANCELLED; }
    else CHECK(0);
    CHECK(UmiComfyTaskObserveJson(&receipt,revision,message,strlen(message),cancel,&out)==expected);
    CHECK(memcmp(&out,&before,sizeof(out))==0);
    umi_cancellation_token_destroy(cancel); return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2); const char *mode=argv[1];
    if(strncmp(mode,"task-",5)==0)return task_case(mode);
    const char *message=started; char buffer[1024];
    UmiComfyEventKind kind=UMI_COMFY_EVENT_START; UmiStatus expected=UMI_STATUS_OK;
    UmiCancellationToken *cancel=NULL;
    if(strcmp(mode,"start")==0) {}
    else if(strcmp(mode,"progress")==0) { kind=UMI_COMFY_EVENT_PROGRESS; message="{\"type\":\"progress\",\"data\":{\"prompt_id\":\"job-1\",\"node\":\"1\",\"value\":2,\"max\":4}}"; }
    else if(strcmp(mode,"queue")==0) { kind=UMI_COMFY_EVENT_QUEUE; message=queue; }
    else if(strcmp(mode,"executing")==0) { kind=UMI_COMFY_EVENT_EXECUTING; message="{\"type\":\"executing\",\"data\":{\"prompt_id\":\"job-1\",\"node\":\"1\"}}"; }
    else if(strcmp(mode,"executing-null")==0) { kind=UMI_COMFY_EVENT_EXECUTING; message="{\"type\":\"executing\",\"data\":{\"prompt_id\":\"job-1\",\"node\":null}}"; }
    else if(strcmp(mode,"ui-output")==0) { kind=UMI_COMFY_EVENT_UI_OUTPUT; message=ui_output; }
    else if(strcmp(mode,"cached")==0) { kind=UMI_COMFY_EVENT_CACHED; message="{\"type\":\"execution_cached\",\"data\":{\"prompt_id\":\"job-1\",\"nodes\":[\"1\",\"2\"]}}"; }
    else if(strcmp(mode,"success")==0) { kind=UMI_COMFY_EVENT_SUCCESS; message=succeeded; }
    else if(strcmp(mode,"error")==0) { kind=UMI_COMFY_EVENT_ERROR; message="{\"type\":\"execution_error\",\"data\":{\"prompt_id\":\"job-1\",\"exception_message\":\"not retained\"}}"; }
    else if(strcmp(mode,"interrupted")==0) { kind=UMI_COMFY_EVENT_INTERRUPTED; message="{\"type\":\"execution_interrupted\",\"data\":{\"prompt_id\":\"job-1\",\"node_id\":\"1\"}}"; }
    else if(strcmp(mode,"unknown")==0) { message="{\"type\":\"future\",\"data\":{}}"; expected=UMI_STATUS_NOT_IMPLEMENTED; }
    else if(strcmp(mode,"duplicate")==0) { message="{\"type\":\"execution_start\",\"data\":{\"prompt_id\":\"job-1\",\"prompt_\\u0069d\":\"job-2\"}}"; expected=UMI_STATUS_ALREADY_EXISTS; }
    else if(strcmp(mode,"negative")==0) { message="{\"type\":\"status\",\"data\":{\"exec_info\":{\"queue_remaining\":-1}}}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"over-progress")==0) { message="{\"type\":\"progress\",\"data\":{\"prompt_id\":\"job-1\",\"node\":\"1\",\"value\":5,\"max\":4}}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"wrong-node")==0) { message="{\"type\":\"executing\",\"data\":{\"prompt_id\":\"job-1\",\"node\":1}}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"missing-prompt")==0) { message="{\"type\":\"execution_success\",\"data\":{}}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"null-data")==0) { message="{\"type\":\"execution_success\",\"data\":null}"; expected=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"long-prompt")==0) { char id[UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY+1U]; memset(id,'a',sizeof(id)); id[sizeof(id)-1U]='\0'; CHECK(snprintf(buffer,sizeof(buffer),"{\"type\":\"execution_start\",\"data\":{\"prompt_id\":\"%s\"}}",id)>0); message=buffer; expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(strcmp(mode,"cancel")==0) { CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK); umi_cancellation_token_request(cancel); expected=UMI_STATUS_CANCELLED; }
    else if(strcmp(mode,"nulls")==0) {
        UmiComfyEvent value;
        CHECK(UmiComfyEventDecode(NULL,1,NULL,&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyEventDecode(started,0,NULL,&value)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyEventDecode(started,sizeof(started)-1,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyEventDecode(started,(size_t)UMI_COMFY_EVENT_MAX_BYTES+1,NULL,&value)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiComfyTaskObserveJson(NULL,1,started,sizeof(started)-1,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT); return 0;
    } else CHECK(0);
    UmiComfyEvent out,before; memset(&out,0xa5,sizeof(out)); memcpy(&before,&out,sizeof(out));
    CHECK(UmiComfyEventDecode(message,strlen(message),cancel,&out)==expected);
    if(expected!=UMI_STATUS_OK) CHECK(memcmp(&out,&before,sizeof(out))==0);
    else {
        CHECK(out.kind==kind);
        if(kind==UMI_COMFY_EVENT_QUEUE) CHECK(out.queue_remaining==2 && out.prompt_id[0]=='\0');
        else CHECK(strcmp(out.prompt_id,"job-1")==0);
        if(kind==UMI_COMFY_EVENT_PROGRESS) CHECK(out.value==2 && out.maximum==4);
        if(strcmp(mode,"executing-null")==0) CHECK(out.node_is_null==1 && out.node_id[0]=='\0');
    }
    umi_cancellation_token_destroy(cancel); return 0;
}
