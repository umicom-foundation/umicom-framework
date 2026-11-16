/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/provider_connections/task_receipt.c
 *
 * PURPOSE:
 *   Demonstrate remote task uncertainty and a confirmed recovery plan using synthetic observations only.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/provider_connections/tasks.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    UmiProviderConnection connection={0};
    strcpy(connection.id,"video");strcpy(connection.provider_id,"example");
    strcpy(connection.label,"Synthetic video provider");
    strcpy(connection.endpoint,"https://api.example.invalid/tasks");strcpy(connection.model,"synthetic-video");
    connection.route=UMI_PROVIDER_CONNECTION_HTTPS;connection.timeout_ms=30000U;connection.enabled=true;
    UmiProviderTaskContext context={0};strcpy(context.application_id,"media");strcpy(context.profile_id,"lesson");
    strcpy(context.local_id,"scene-one");strcpy(context.operation_id,"video.generate");
    /* These are deterministic example request bytes, not a real provider call. */
    UmiSha256 hash;UmiSha256Init(&hash);
    static const char request[]="synthetic scene request";
    UmiStatus status=UmiSha256Update(&hash,request,sizeof(request)-1U);
    if(status==UMI_STATUS_OK) status=UmiSha256Final(&hash,context.request_sha256);
    UmiProviderTaskReceipt receipt;
    if(status==UMI_STATUS_OK) status=UmiProviderTaskPrepare(&context,&connection,1U,&receipt);
    if(status==UMI_STATUS_OK) status=UmiProviderTaskAdvance(&receipt,receipt.revision,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&receipt);
    /* A real host must commit the uncertain receipt before sending a creation
     * request. This memory-only lesson simulates the provider's response. */
    if(status==UMI_STATUS_OK) status=UmiProviderTaskConfirm(&receipt,receipt.revision,"Synthetic-123",&receipt);
    UmiProviderTaskRecoveryAction action;
    if(status==UMI_STATUS_OK) status=UmiProviderTaskPlanRecovery(&receipt,"media","lesson",&connection,1U,&action);
    if(status==UMI_STATUS_OK && action!=UMI_PROVIDER_TASK_POLL) status=UMI_STATUS_INVALID_STATE;
    if(status==UMI_STATUS_OK) {
        printf("Synthetic confirmed task: %s; recovery plan: poll. No network request was made.\n",receipt.remote_id);
        if(fflush(stdout)!=0 || ferror(stdout)) status=UMI_STATUS_IO_ERROR;
    } else fprintf(stderr,"Receipt planning failed: %s\n",umi_status_text(status));
    return status==UMI_STATUS_OK?0:1;
}
