/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_queue/main.c
 *
 * PURPOSE:
 *   Use the installed Framework SDK to inspect a synthetic queue without executing or cancelling a job.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_queue.h"
#include "umicom/media_generation/comfy_inspection.h"
#include "umicom/platform/native_arguments.h"
#include <stdio.h>
#include <string.h>
/* Keep the original synthetic no-argument workflow while allowing this installed SDK example to exercise the same shared console route as products. The former implementation is retained for owner review. */
#if 0
int main(void)
#endif
static int QueueSample(void)
{
    /* A captured pending list is an observation, not a sorted execution plan. */
    const char sample[]="{\"queue_running\":[[1,\"example-running\",{},{},[]]],"
        "\"queue_pending\":[[5,\"example-later\",{},{},[]],[2.5,\"example-earlier\",{},{},[]]]}";
    UmiProviderConnection connection={0};strcpy(connection.id,"queue-example");
    strcpy(connection.provider_id,"comfyui");strcpy(connection.label,"Queue example");
    strcpy(connection.endpoint,"http://127.0.0.1:8188");connection.enabled=true;
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.timeout_ms=30000;
    UmiComfyQueue *queue=NULL;UmiComfyQueueReview review={0};
    UmiStatus status=UmiComfyQueueCreate(&connection,1U,sample,strlen(sample),NULL,&queue);
    if(status==UMI_STATUS_OK)status=UmiComfyQueueInspect(queue,&review);
    if(status==UMI_STATUS_OK && (review.running_count!=1U || review.pending_count!=2U))status=UMI_STATUS_INVALID_STATE;
    size_t index=0U;UmiComfyQueueEntry item={0};
    if(status==UMI_STATUS_OK)status=UmiComfyQueueFind(queue,"example-earlier",&index);
    if(status==UMI_STATUS_OK)status=UmiComfyQueueAt(queue,index,&item);
    if(status==UMI_STATUS_OK && (item.state!=UMI_COMFY_QUEUE_PENDING || strcmp(item.number_json,"2.5")!=0))status=UMI_STATUS_INVALID_STATE;
    if(status==UMI_STATUS_OK)status=UmiComfyQueueCheckConnection(queue,&connection,1U);
    if(status==UMI_STATUS_OK && UmiComfyQueueCheckConnection(queue,&connection,2U)!=UMI_STATUS_BUSY)status=UMI_STATUS_INVALID_STATE;
    /* Absence has no completion meaning. A real client consults separately
     * bound history and transport evidence before changing its task receipt. */
    if(status==UMI_STATUS_OK && UmiComfyQueueFind(queue,"unknown-job",&index)!=UMI_STATUS_NOT_FOUND)status=UMI_STATUS_INVALID_STATE;
    UmiComfyQueueDestroy(queue);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Umicom queue example: %s\n",umi_status_text(status));return 1;}
    printf("Observed %zu running and %zu pending jobs in synthetic input.\n",review.running_count,review.pending_count);
    puts("A missing job is not a completion receipt; no server was contacted.");
    return fflush(stdout)==0 && !ferror(stdout)?0:1;
}

static int QueueEntry(int argc,char **argv)
{
    if(argc==1)return QueueSample();
    if(argc>=2 && strcmp(argv[1],"comfy")==0)return UmiComfyInspectionCommand(argc-2,argv+2);
    fputs("Use no arguments for the synthetic sample, or comfy queue --help.\n",stderr);return 2;
}
int main(int argc,char **argv)
{
    int result=1;UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,QueueEntry,&result);
    return status==UMI_STATUS_OK?result:1;
}
