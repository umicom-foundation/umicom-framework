/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/provider_connections/stored_tasks.c
 *
 * PURPOSE:
 *   Demonstrate retained submission uncertainty across store reopening without any provider request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/provider_connections/task_store.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
int main(void)
{
    UmiDataServer *server=NULL;UmiProviderTasks *store=NULL;
    UmiProviderConnection connection={0};strcpy(connection.id,"video");strcpy(connection.provider_id,"example");
    strcpy(connection.label,"Example provider");strcpy(connection.endpoint,"https://api.example.invalid/tasks");
    connection.route=UMI_PROVIDER_CONNECTION_HTTPS;connection.timeout_ms=30000U;connection.enabled=true;
    UmiProviderTaskContext context={0};strcpy(context.application_id,"media");strcpy(context.profile_id,"lesson");
    strcpy(context.local_id,"scene-one");strcpy(context.operation_id,"video.generate");
    static const char request[]="Synthetic request; never submitted";
    UmiSha256 hash;UmiSha256Init(&hash);
    UmiStatus status=UmiSha256Update(&hash,request,sizeof(request)-1U);
    if(status==UMI_STATUS_OK) status=UmiSha256Final(&hash,context.request_sha256);
    UmiProviderTaskReceipt receipt;
    if(status==UMI_STATUS_OK) status=UmiProviderTaskPrepare(&context,&connection,1U,&receipt);
    if(status==UMI_STATUS_OK) status=umi_data_server_create_memory(&server);
    if(status==UMI_STATUS_OK) status=UmiProviderTasksOpen(server,"media","lesson",&store);
    if(status==UMI_STATUS_OK) status=UmiProviderTasksCreate(store,&receipt);
    if(status==UMI_STATUS_OK) status=UmiProviderTasksAdvance(store,"scene-one",1U,
        UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&receipt);
    /* This lesson intentionally stops before any external effect. Reopening
     * a store does not make its uncertain task eligible for resubmission. The
     * memory server survives this store replacement, not process termination. */
    UmiProviderTasksDestroy(store);store=NULL;
    if(status==UMI_STATUS_OK) status=UmiProviderTasksOpen(server,"media","lesson",&store);
    if(status==UMI_STATUS_OK) status=UmiProviderTasksRead(store,"scene-one",&receipt);
    if(status==UMI_STATUS_OK) {
        if(receipt.phase!=UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN) status=UMI_STATUS_INVALID_STATE;
        else if(printf("Retained uncertain task at revision %" PRIu64 "; no request sent.\n",receipt.revision)<0 ||
            fflush(stdout)==EOF) status=UMI_STATUS_IO_ERROR;
    }
    if(status==UMI_STATUS_OK) {
        UmiProviderTaskQuery query={0};query.limit=8U;query.phase=UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN;
        UmiProviderTaskPage page;status=UmiProviderTasksReadPage(store,&query,&page);
        if(status==UMI_STATUS_OK && (page.count!=1U || page.matching_count!=1U ||
            strcmp(page.items[0].context.local_id,"scene-one")!=0)) status=UMI_STATUS_INVALID_STATE;
        if(status==UMI_STATUS_OK && (puts("Found one retained task in the scoped catalogue.")<0 ||
            fflush(stdout)==EOF)) status=UMI_STATUS_IO_ERROR;
    }
    UmiProviderTasksDestroy(store);umi_data_server_destroy(server);
    if(status!=UMI_STATUS_OK) fprintf(stderr,"Stored task lesson failed: %d\n",(int)status);
    return status==UMI_STATUS_OK?0:1;
}
