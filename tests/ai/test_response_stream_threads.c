/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_response_stream_threads.c
 *
 * PURPOSE:
 *   Exercise copied AI stream observations, cancellation and competing sequence admission with real threads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/response_stream.h"
#include "umicom/base/utf8.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
static const UmiAiResponseStreamIdentity key={9U,2U};
typedef struct Worker {
    UmiAiResponseStream *stream;
    UmiMutex *mutex;
    UmiCondition *condition;
    int ready,released,competing;
    atomic_int done;
    UmiStatus status;
} Worker;
static int Run(void *data)
{
    Worker *worker=data;
    worker->status=worker->competing?UMI_STATUS_OK:
        UmiAiResponseStreamPush(worker->stream,key,1U,(const unsigned char *)"\xe2",1U,0);
    if(worker->status!=UMI_STATUS_OK){atomic_store(&worker->done,1);return 1;}
    UmiStatus status=umi_mutex_lock(worker->mutex);
    if(status!=UMI_STATUS_OK){atomic_store(&worker->done,1);return 2;}
    worker->ready=1;(void)umi_condition_broadcast(worker->condition);
    while(!worker->released && status==UMI_STATUS_OK)
        status=umi_condition_wait_for(worker->condition,worker->mutex,10000U);
    (void)umi_mutex_unlock(worker->mutex);
    if(status!=UMI_STATUS_OK){atomic_store(&worker->done,1);return 3;}
    if(worker->competing)
        worker->status=UmiAiResponseStreamPush(worker->stream,key,1U,(const unsigned char *)"X",1U,1);
    else{
        const unsigned char scalar[]={0xe2U,0x82U,0xacU};
        for(uint64_t sequence=2U;sequence<=3000U;++sequence){
            worker->status=UmiAiResponseStreamPush(worker->stream,key,sequence,
                scalar+(size_t)((sequence-1U)%3U),1U,sequence==3000U);
            if(worker->status!=UMI_STATUS_OK)break;
        }
    }
    atomic_store_explicit(&worker->done,1,memory_order_release);
    return 0;
}
static int Start(Worker *worker,UmiThread **thread)
{
    atomic_init(&worker->done,0);
    CHECK(umi_mutex_create(&worker->mutex)==UMI_STATUS_OK);
    CHECK(umi_condition_create(&worker->condition)==UMI_STATUS_OK);
    CHECK(umi_thread_start(Run,worker,thread)==UMI_STATUS_OK);
    CHECK(umi_mutex_lock(worker->mutex)==UMI_STATUS_OK);
    UmiStatus status=UMI_STATUS_OK;
    while(!worker->ready && status==UMI_STATUS_OK)
        status=umi_condition_wait_for(worker->condition,worker->mutex,10000U);
    CHECK(umi_mutex_unlock(worker->mutex)==UMI_STATUS_OK && status==UMI_STATUS_OK);
    return 0;
}
static int Release(Worker *worker)
{
    CHECK(umi_mutex_lock(worker->mutex)==UMI_STATUS_OK);
    worker->released=1;
    CHECK(umi_condition_broadcast(worker->condition)==UMI_STATUS_OK);
    CHECK(umi_mutex_unlock(worker->mutex)==UMI_STATUS_OK);return 0;
}
static int Join(Worker *worker,UmiThread **thread)
{
    int result=99;
    CHECK(umi_thread_join(*thread,&result)==UMI_STATUS_OK && result==0);
    CHECK(UmiThreadRelease(thread)==UMI_STATUS_OK);
    umi_condition_destroy(worker->condition);umi_mutex_destroy(worker->mutex);
    return 0;
}

static int RangeReader(void)
{
    UmiAiResponseStream *stream = NULL;
    UmiThread *thread = NULL;
    CHECK(UmiAiResponseStreamCreate(key, 4096U, &stream) == UMI_STATUS_OK);
    Worker worker = {.stream=stream};
    CHECK(Start(&worker, &thread) == 0);
    CHECK(Release(&worker) == 0);
    size_t offset = 0U;
    UmiAiResponseStreamSnapshot snapshot = {0};
    for (unsigned attempt = 0U; attempt < 10000U; ++attempt) {
        char *text = NULL; size_t copied = 0U;
        CHECK(UmiAiResponseStreamCopyRange(stream, offset, 7U,
            &text, &copied, &snapshot) == UMI_STATUS_OK);
        CHECK(copied <= 7U && copied % 3U == 0U && strlen(text) == copied &&
            offset + copied <= snapshot.text_bytes && snapshot.pending_bytes <= 2U);
        CHECK(UmiUtf8Validate((const unsigned char *)text, copied, NULL));
        for (size_t index = 0U; index < copied; index += 3U)
            CHECK(memcmp(text + index, "\xe2\x82\xac", 3U) == 0);
        offset += copied;
        UmiAiResponseStreamTextFree(text);
        if (snapshot.state == UMI_AI_RESPONSE_STREAM_COMPLETE && offset == snapshot.text_bytes) break;
        if (copied == 0U) umi_thread_sleep_ms(1U);
    }
    CHECK(Join(&worker, &thread) == 0);
    CHECK(worker.status == UMI_STATUS_OK && offset == 3000U &&
        snapshot.state == UMI_AI_RESPONSE_STREAM_COMPLETE);
    UmiAiResponseStreamDestroy(stream);
    return 0;
}

int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(strcmp(argv[1],"producer-range-reader")==0)return RangeReader();
    int compete=strcmp(argv[1],"competing-sequence")==0;
    int cancel=strcmp(argv[1],"cancel-producer")==0;
    if(!compete && !cancel && strcmp(argv[1],"producer-reader")!=0)return 2;
    UmiAiResponseStream *stream=NULL;UmiThread *thread=NULL,*other_thread=NULL;
    CHECK(UmiAiResponseStreamCreate(key,4096U,&stream)==UMI_STATUS_OK);
    Worker worker={.stream=stream,.competing=compete},other={.stream=stream,.competing=1};
    CHECK(Start(&worker,&thread)==0);
    UmiAiResponseStreamSnapshot snapshot;char *text=NULL;
    CHECK(UmiAiResponseStreamCopy(stream,&text,&snapshot)==UMI_STATUS_OK);
    CHECK(strcmp(text,"")==0 && snapshot.pending_bytes==(compete?0U:1U));
    UmiAiResponseStreamTextFree(text);
    if(compete)CHECK(Start(&other,&other_thread)==0);
    if(cancel)CHECK(UmiAiResponseStreamCancel(stream,key)==UMI_STATUS_OK);
    CHECK(Release(&worker)==0);
    if(compete)CHECK(Release(&other)==0);
    uint64_t last=0U;
    while(!atomic_load_explicit(&worker.done,memory_order_acquire)){
        CHECK(UmiAiResponseStreamCopy(stream,&text,&snapshot)==UMI_STATUS_OK);
        CHECK(snapshot.last_sequence>=last && strlen(text)==snapshot.text_bytes &&
            snapshot.pending_bytes<=3U && snapshot.accepted_bytes==snapshot.text_bytes+snapshot.pending_bytes);
        CHECK(UmiUtf8Validate((const unsigned char *)text,snapshot.text_bytes,NULL));
        if(!compete)CHECK(snapshot.text_bytes%3U==0U);
        last=snapshot.last_sequence;
        UmiAiResponseStreamTextFree(text);
        umi_thread_sleep_ms(1U);
    }
    CHECK(Join(&worker,&thread)==0);
    if(compete)CHECK(Join(&other,&other_thread)==0);
    CHECK(UmiAiResponseStreamCopy(stream,&text,&snapshot)==UMI_STATUS_OK);
    if(compete){
        CHECK((worker.status==UMI_STATUS_OK && other.status==UMI_STATUS_INVALID_STATE) ||
            (other.status==UMI_STATUS_OK && worker.status==UMI_STATUS_INVALID_STATE));
        CHECK(strcmp(text,"X")==0 && snapshot.state==UMI_AI_RESPONSE_STREAM_COMPLETE);
    }else if(cancel){
        CHECK(worker.status==UMI_STATUS_INVALID_STATE && strcmp(text,"")==0);
        CHECK(snapshot.state==UMI_AI_RESPONSE_STREAM_CANCELLED && snapshot.pending_bytes==1U);
    }else{
        CHECK(worker.status==UMI_STATUS_OK && snapshot.state==UMI_AI_RESPONSE_STREAM_COMPLETE);
        CHECK(snapshot.text_bytes==3000U && snapshot.last_sequence==3000U);
        for(size_t i=0U;i<snapshot.text_bytes;i+=3U)CHECK(memcmp(text+i,"\xe2\x82\xac",3U)==0);
    }
    /* Worker callbacks and native thread teardown finish before owner release. */
    UmiAiResponseStreamDestroy(stream);
    CHECK(strlen(text)==snapshot.text_bytes);
    UmiAiResponseStreamTextFree(text);return 0;
}
