/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_prompt/main.c
 *
 * PURPOSE:
 *   Teach independent SDK request review and receipt identity without submitting a workflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_prompt.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    /* ExampleNode is a synthetic type, not an installed ComfyUI node claim.
     * Preparation does not contact a server or render an image. */
    const char workflow[]="{\"one\":{\"class_type\":\"ExampleNode\",\"inputs\":{\"message\":\"Learn native request ownership\"}}}";
    UmiComfyPromptDraft draft={workflow,sizeof(workflow)-1,"sdk-example"};
    UmiProviderConnection connection={0};
    strcpy(connection.id,"local-comfy");strcpy(connection.provider_id,"comfyui");
    strcpy(connection.label,"Local ComfyUI exercise");
    strcpy(connection.endpoint,"http://127.0.0.1:8188");
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;
    connection.timeout_ms=30000;connection.enabled=true;
    UmiComfyPrompt *prompt=NULL;
    UmiStatus status=UmiComfyPromptCreate(&connection,1,&draft,NULL,&prompt);
    UmiComfyPromptReview review;const char *body=NULL;
    if(status==UMI_STATUS_OK)status=UmiComfyPromptInspect(prompt,&review,&body);
    UmiProviderTaskReceipt task;
    if(status==UMI_STATUS_OK)status=UmiComfyPromptPrepareTask(prompt,"sdk-demo","local","prompt-example",&task);
    if(status==UMI_STATUS_OK && (task.phase!=UMI_PROVIDER_TASK_PREPARED ||
        memcmp(task.context.request_sha256,review.request_sha256,UMI_SHA256_BYTES)!=0))
        status=UMI_STATUS_INVALID_STATE;
    if(status==UMI_STATUS_OK){
        char digest[UMI_SHA256_HEX_CAPACITY];UmiSha256Hex(review.request_sha256,digest);
        /* Only this deliberately synthetic fixture is printed. Real hosts must
         * review private prompt data in the chosen profile instead of logging it. */
        printf("Prepared %zu node(s), %zu bytes, SHA-256 %s\n",review.node_count,review.body_bytes,digest);
        puts("Receipt remains PREPARED. No server contacted; no media generated.");
    }
    UmiComfyPromptDestroy(prompt);return status==UMI_STATUS_OK?0:1;
}
