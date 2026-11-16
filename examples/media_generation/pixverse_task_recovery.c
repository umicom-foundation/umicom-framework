/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/media_generation/pixverse_task_recovery.c
 *
 * PURPOSE:
 *   Demonstrate a stored video receipt and explicit status recovery using an offline transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/pixverse_tasks.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
/* This transport is deliberately synthetic. The native plan and result parser
 * still run, but no provider, credentials, media file or network is accessed. */
static UmiStatus ExampleExchange(void *context,const char *url,const char *body,const char *trace,
    const char *key,const UmiCancellationToken *cancel,char *reply,size_t capacity,size_t *length,unsigned *http)
{
    (void)context;(void)url;(void)trace;(void)key;(void)cancel;
    const char *value=body?"{\"ErrCode\":0,\"Resp\":{\"video_id\":42}}":
        "{\"ErrCode\":0,\"Resp\":{\"id\":42,\"status\":5}}";
    size_t count=strlen(value);if(count>=capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(reply,value,count);*length=count;*http=200U;return UMI_STATUS_OK;
}
int main(void)
{
    UmiProviderConnection connection={0};strcpy(connection.id,"video");strcpy(connection.provider_id,"pixverse");
    strcpy(connection.label,"Offline video lesson");strcpy(connection.endpoint,"https://app-api.pixverse.ai/openapi/v2/");
    strcpy(connection.model,"v6");connection.route=UMI_PROVIDER_CONNECTION_HTTPS;
    connection.timeout_ms=30000U;connection.enabled=true;
    UmiPixVerseDraft draft={0};draft.operation=UMI_PIXVERSE_CREATE_VIDEO;
    strcpy(draft.trace_id,"93cb2fa6-e5da-4c69-9588-94ca9839f238");strcpy(draft.prompt,"A quiet sea");
    strcpy(draft.aspect_ratio,"16:9");draft.seconds=5U;draft.quality=720U;
    UmiDataServer *server=NULL;UmiProviderTasks *store=NULL;UmiPixVersePlan *create=NULL,*poll=NULL;
    UmiProviderTaskReceipt receipt;UmiPixVerseResult result;
    UmiStatus status=UmiPixVersePrepare(&draft,&create);
    if(status==UMI_STATUS_OK) status=UmiPixVerseTaskPrepare(create,"media","lesson","scene-one",&connection,7U,&receipt);
    if(status==UMI_STATUS_OK) status=umi_data_server_create_memory(&server);
    if(status==UMI_STATUS_OK) status=UmiProviderTasksOpen(server,"media","lesson",&store);
    if(status==UMI_STATUS_OK) status=UmiProviderTasksCreate(store,&receipt);
    /* Persist uncertainty before transport. A real host uses durable storage,
     * exact-plan approval and one effect owner. Do not submit if this fails. */
    if(status==UMI_STATUS_OK) status=UmiProviderTasksAdvance(store,"scene-one",1U,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&receipt);
    if(status==UMI_STATUS_OK) {
        UmiStatus execution=UmiPixVerseExecuteWithTransport(create,true,"fixture-key",NULL,ExampleExchange,NULL,&result);
        status=UmiPixVerseTaskRecordResult(store,"scene-one",receipt.revision,create,execution,&result,&receipt);
    }
    /* Reopen the store while its memory server survives. This demonstrates
     * store lifetime only; process restart requires a persistent backend. */
    UmiProviderTasksDestroy(store);store=NULL;
    if(status==UMI_STATUS_OK) status=UmiProviderTasksOpen(server,"media","lesson",&store);
    if(status==UMI_STATUS_OK) status=UmiProviderTasksRead(store,"scene-one",&receipt);
    if(status==UMI_STATUS_OK) status=UmiPixVerseTaskStatusPlan(&receipt,"media","lesson",&connection,7U,
        "93cb2fa6-e5da-4c69-9588-94ca9839f239",&poll);
    if(status==UMI_STATUS_OK) {
        UmiStatus execution=UmiPixVerseExecuteWithTransport(poll,true,"fixture-key",NULL,ExampleExchange,NULL,&result);
        status=UmiPixVerseTaskRecordResult(store,"scene-one",receipt.revision,poll,execution,&result,&receipt);
    }
    if(status==UMI_STATUS_OK && (receipt.phase!=UMI_PROVIDER_TASK_RUNNING || receipt.revision!=4U)) status=UMI_STATUS_INVALID_STATE;
    if(status==UMI_STATUS_OK && (printf("Offline job %s retained at revision %" PRIu64 "; no provider request sent.\n",
        receipt.remote_id,receipt.revision)<0 || fflush(stdout)==EOF)) status=UMI_STATUS_IO_ERROR;
    UmiPixVerseDestroy(poll);UmiPixVerseDestroy(create);UmiProviderTasksDestroy(store);umi_data_server_destroy(server);
    if(status!=UMI_STATUS_OK) fprintf(stderr,"Video task lesson failed: %d\n",(int)status);
    return status==UMI_STATUS_OK?0:1;
}
