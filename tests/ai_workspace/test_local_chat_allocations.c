/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_chat_allocations.c
 *
 * PURPOSE:
 *   Inject only the real codec conversion allocations and verify precise failure status, cleanup and retry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_chat_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
static unsigned allocation_call,fail_at;
static void *owned[8];
static int tracking_error;
static void *CodecMalloc(size_t size)
{
    ++allocation_call;
    if(fail_at!=0U && allocation_call==fail_at)return NULL;
    void *pointer=malloc(size);
    if(pointer==NULL)return NULL;
    for(size_t i=0U;i<sizeof owned/sizeof owned[0];++i){
        if(owned[i]==NULL){owned[i]=pointer;return pointer;}
    }
    tracking_error=1;free(pointer);return NULL;
}
static void CodecFree(void *pointer)
{
    if(pointer==NULL)return;
    for(size_t i=0U;i<sizeof owned/sizeof owned[0];++i){
        if(owned[i]==pointer){owned[i]=NULL;free(pointer);return;}
    }
    tracking_error=1;
}
static int LiveAllocations(void)
{
    int count=0;
    for(size_t i=0U;i<sizeof owned/sizeof owned[0];++i)if(owned[i]!=NULL)++count;
    return count;
}
/* Instrument this source translation unit only, so shared or static json-c
 * allocator choices do not change which conversion allocation fails. The
 * production codec is included once, with its actual native text validator. */
#define malloc CodecMalloc
#define free CodecFree
#include "../../src/ai_workspace/local_chat_codec.c"
#undef malloc
#undef free

int main(int argc,char **argv)
{
    if(argc!=3 || (strcmp(argv[2],"1")!=0 && strcmp(argv[2],"2")!=0))return 2;
    const char *mode=argv[1];
    if(strcmp(mode,"chat")!=0 && strcmp(mode,"health")!=0 && strcmp(mode,"delta")!=0)return 2;
    UmiAiRequest *request=calloc(1U,sizeof *request);CHECK(request!=NULL);
    strcpy(request->request_id,"allocation.test");strcpy(request->model_id,"test-model");
    const char *chat="{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"ok\"},\"finish_reason\":\"stop\"}]}";
    const char *health="{\"status\":\"ok\"}";
    const char *delta="{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"ok\"},\"finish_reason\":null}]}";
    for(unsigned attempt=0;attempt<2U;++attempt){
        allocation_call=0U;fail_at=attempt==0U?(unsigned)(argv[2][0]-'0'):0U;
        UmiStatus wanted=attempt==0U?UMI_STATUS_OUT_OF_MEMORY:UMI_STATUS_OK,status;
        if(strcmp(mode,"chat")==0){
            UmiAiResponse output,before;memset(&output,0xa5,sizeof output);memcpy(&before,&output,sizeof before);
            status=AwLocalChatDecode(request,chat,strlen(chat),&output);
            CHECK(status==wanted);
            if(attempt==0U)CHECK(memcmp(&output,&before,sizeof output)==0);
            else CHECK(strcmp(output.text,"ok")==0);
        }else if(strcmp(mode,"health")==0){
            UmiAiLocalHealth output,before;memset(&output,0xa5,sizeof output);memcpy(&before,&output,sizeof before);
            status=AwLocalHealthDecode(200L,health,strlen(health),&output);
            CHECK(status==wanted);
            if(attempt==0U)CHECK(memcmp(&output,&before,sizeof output)==0);
            else CHECK(output.endpoint_reports_ready);
        }else{
            AwLocalChatDelta output,before;memset(&output,0xa5,sizeof output);memcpy(&before,&output,sizeof before);
            status=AwLocalChatDeltaDecode(request,delta,strlen(delta),&output);
            CHECK(status==wanted);
            if(attempt==0U)CHECK(memcmp(&output,&before,sizeof output)==0);
            else CHECK(strcmp(output.text,"ok")==0);
        }
        CHECK(allocation_call==(attempt==0U?fail_at:2U));
        CHECK(!tracking_error && LiveAllocations()==0);
    }
    free(request);return 0;
}
