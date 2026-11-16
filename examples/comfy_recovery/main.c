/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_recovery/main.c
 *
 * PURPOSE:
 *   Demonstrate offline submission confirmation and history recovery through the installed native Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_recovery.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    /* Synthetic request/response fixtures exercise protocol ownership only.
     * No server is contacted and no model, image or output file is created. */
    const char request[]="{\"prompt\":{}}";
    const char reply[]="{\"prompt_id\":\"demo-job\",\"number\":1,\"node_errors\":{}}";
    const char history_json[]="{\"demo-job\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":[{\"filename\":\"frame.png\",\"subfolder\":\"demo\",\"type\":\"output\"}]}}}}";
    UmiProviderConnection connection={0}; UmiProviderTaskContext context={0};
    strcpy(connection.id,"local-comfy"); strcpy(connection.provider_id,"comfyui");
    strcpy(connection.label,"Offline ComfyUI example");
    strcpy(connection.endpoint,"http://127.0.0.1:8188"); strcpy(connection.model,"workflow");
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK; connection.timeout_ms=30000; connection.enabled=true;
    strcpy(context.application_id,"sdk-demo"); strcpy(context.profile_id,"local");
    strcpy(context.local_id,"example"); strcpy(context.operation_id,UMI_COMFY_TASK_OPERATION);
    UmiSha256 digest; UmiSha256Init(&digest);
    if(UmiSha256Update(&digest,request,sizeof(request)-1)!=UMI_STATUS_OK ||
       UmiSha256Final(&digest,context.request_sha256)!=UMI_STATUS_OK) return 1;
    UmiProviderTaskReceipt task;
    if(UmiProviderTaskPrepare(&context,&connection,1,&task)!=UMI_STATUS_OK) return 1;
    if(UmiProviderTaskAdvance(&task,task.revision,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&task)!=UMI_STATUS_OK) return 1;
    /* A real host persists this uncertain receipt before sending. This example
     * has no transport or durable store and makes no recovery-durability claim. */
    if(UmiComfyTaskConfirmJson(&task,task.revision,200,reply,sizeof(reply)-1,NULL,&task)!=UMI_STATUS_OK) return 1;
    UmiComfyHistory *history=NULL;
    if(UmiComfyHistoryCreate(history_json,sizeof(history_json)-1,task.remote_id,NULL,&history)!=UMI_STATUS_OK) return 1;
    UmiStatus status=UmiComfyTaskObserveHistory(&task,task.revision,history,&task);
    UmiComfyImageReference image;
    if(status==UMI_STATUS_OK) status=UmiComfyHistoryImageAt(history,0,&image);
    if(status==UMI_STATUS_OK && task.phase==UMI_PROVIDER_TASK_SUCCEEDED && task.output_bytes==0)
        puts("Synthetic provider success; one image reference; no output downloaded or stored.");
    else status=UMI_STATUS_INVALID_STATE;
    UmiComfyHistoryDestroy(history);
    return status==UMI_STATUS_OK?0:1;
}
