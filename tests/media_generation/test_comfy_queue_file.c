/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_queue_file.c
 *
 * PURPOSE:
 *   Verify captured queue files preserve ownership on admission and cancellation failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_queue_file.h"
#include "umicom/platform/native_arguments.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int QueueFileTest(int argc,char **argv)
{
    CHECK(argc==4);const char *mode=argv[1],*document=argv[2],*input=argv[3];
    UmiComfyQueue *queue=NULL;UmiStatus expected=UMI_STATUS_OK;
    UmiCancellationToken *cancel=NULL;
    if(!strcmp(mode,"disabled"))expected=UMI_STATUS_PERMISSION_DENIED;
    else if(!strcmp(mode,"malformed-document") || !strcmp(mode,"malformed-queue"))expected=UMI_STATUS_PARSE_ERROR;
    else if(!strcmp(mode,"missing-file"))expected=UMI_STATUS_NOT_FOUND;
    else if(!strcmp(mode,"directory"))expected=UMI_STATUS_IO_ERROR;
    else if(!strcmp(mode,"queue-limit") || !strcmp(mode,"document-limit"))expected=UMI_STATUS_CAPACITY_EXCEEDED;
    else if(!strcmp(mode,"relative-before-read")){input="relative.json";expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"null-document")){document=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"null-queue")){input=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"null-output"))expected=UMI_STATUS_INVALID_ARGUMENT;
    else if(!strcmp(mode,"cancel")){
        CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);expected=UMI_STATUS_CANCELLED;
    }else if(!strcmp(mode,"occupied")){
        CHECK(UmiComfyQueueFileRead(document,input,NULL,&queue)==UMI_STATUS_OK);
        expected=UMI_STATUS_INVALID_ARGUMENT;
    }else CHECK(!strcmp(mode,"complete") || !strcmp(mode,"empty") || !strcmp(mode,"unicode-path"));
    UmiComfyQueue *before=queue;
    UmiStatus status=UmiComfyQueueFileRead(document,input,cancel,!strcmp(mode,"null-output")?NULL:&queue);
    umi_cancellation_token_destroy(cancel);CHECK(status==expected);
    if(status==UMI_STATUS_OK){
        UmiComfyQueueReview review;CHECK(UmiComfyQueueInspect(queue,&review)==UMI_STATUS_OK);
        size_t count=!strcmp(mode,"empty")?0U:3U;
        CHECK(UmiComfyQueueCount(queue)==count && review.connection_revision==7U);
        CHECK(review.running_count==(!strcmp(mode,"empty")?0U:1U));
        CHECK(review.pending_count==(!strcmp(mode,"empty")?0U:2U));
    }else CHECK(queue==before);
    UmiComfyQueueDestroy(queue);return 0;
}
int main(int argc,char **argv)
{
    int result=1;UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,QueueFileTest,&result);
    return status==UMI_STATUS_OK?result:1;
}
