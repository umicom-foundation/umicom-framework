/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_response_stream_faults.c
 *
 * PURPOSE:
 *   Verify response-stream allocation cleanup and native mutex error publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/response_stream.h"
#include "umicom/platform/threading.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
static int fail_calloc,fail_malloc,fail_create,fail_lock,fail_unlock,tracking,tracking_error;
static void *owned[16];
static unsigned live,failures;
void *__real_calloc(size_t,size_t);
void *__real_malloc(size_t);
void __real_free(void *);
UmiStatus __real_umi_mutex_create(UmiMutex **);
UmiStatus __real_umi_mutex_lock(UmiMutex *);
UmiStatus __real_umi_mutex_unlock(UmiMutex *);
static void Track(void *pointer)
{
    if(!tracking || pointer==NULL)return;
    for(size_t i=0U;i<sizeof owned/sizeof owned[0];++i)
        if(owned[i]==NULL){owned[i]=pointer;++live;return;}
    tracking_error=1;
}
void *__wrap_calloc(size_t count,size_t size)
{
    if(fail_calloc){fail_calloc=0;++failures;return NULL;}
    void *pointer=__real_calloc(count,size);Track(pointer);return pointer;
}
void *__wrap_malloc(size_t size)
{
    if(fail_malloc){fail_malloc=0;++failures;return NULL;}
    void *pointer=__real_malloc(size);Track(pointer);return pointer;
}
void __wrap_free(void *pointer)
{
    if(pointer!=NULL)for(size_t i=0U;i<sizeof owned/sizeof owned[0];++i)
        if(owned[i]==pointer){owned[i]=NULL;--live;break;}
    __real_free(pointer);
}
UmiStatus __wrap_umi_mutex_create(UmiMutex **out)
{
    if(fail_create){fail_create=0;++failures;return UMI_STATUS_UNAVAILABLE;}
    return __real_umi_mutex_create(out);
}
UmiStatus __wrap_umi_mutex_lock(UmiMutex *mutex)
{
    if(fail_lock){fail_lock=0;++failures;return UMI_STATUS_UNAVAILABLE;}
    return __real_umi_mutex_lock(mutex);
}
UmiStatus __wrap_umi_mutex_unlock(UmiMutex *mutex)
{
    UmiStatus status=__real_umi_mutex_unlock(mutex);
    if(status==UMI_STATUS_OK && fail_unlock){fail_unlock=0;++failures;return UMI_STATUS_IO_ERROR;}
    return status;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *mode=argv[1];
    const UmiAiResponseStreamIdentity key={4U,8U};
    UmiAiResponseStream *stream=NULL;
    UmiAiResponseStreamSnapshot before,after;
    memset(&after,0x5a,sizeof after);before=after;
    tracking=1;
    if(strcmp(mode,"owner-allocation")==0)fail_calloc=1;
    else if(strcmp(mode,"buffer-allocation")==0)fail_malloc=1;
    else if(strcmp(mode,"mutex-create")==0)fail_create=1;
    UmiStatus status=UmiAiResponseStreamCreate(key,64U,&stream);
    if(strcmp(mode,"owner-allocation")==0 || strcmp(mode,"buffer-allocation")==0 || strcmp(mode,"mutex-create")==0){
        CHECK(status==(strcmp(mode,"mutex-create")==0?UMI_STATUS_UNAVAILABLE:UMI_STATUS_OUT_OF_MEMORY));
        CHECK(stream==NULL && failures==1U && live==0U && !tracking_error);
        tracking=0;return 0;
    }
    CHECK(status==UMI_STATUS_OK);
    char sentinel[]="unchanged";char *text=sentinel;
    if(strncmp(mode,"range-",6U)==0){
        size_t copied=91U;
        if(strcmp(mode,"range-allocation")==0)fail_malloc=1;
        else if(strcmp(mode,"range-lock")==0)fail_lock=1;
        else if(strcmp(mode,"range-unlock")==0)fail_unlock=1;
        else return 2;
        UmiStatus expected=fail_malloc?UMI_STATUS_OUT_OF_MEMORY:fail_lock?UMI_STATUS_UNAVAILABLE:UMI_STATUS_IO_ERROR;
        CHECK(UmiAiResponseStreamCopyRange(stream,0U,4U,&text,&copied,&after)==expected);
        CHECK(text==sentinel && copied==91U && memcmp(&before,&after,sizeof after)==0);
    }else if(strcmp(mode,"copy-allocation")==0){
        fail_malloc=1;
        CHECK(UmiAiResponseStreamCopy(stream,&text,&after)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(text==sentinel && memcmp(&before,&after,sizeof after)==0);
    }else if(strcmp(mode,"push-lock")==0){
        fail_lock=1;
        CHECK(UmiAiResponseStreamPush(stream,key,1U,(const unsigned char *)"A",1U,0)==UMI_STATUS_UNAVAILABLE);
        CHECK(UmiAiResponseStreamInspect(stream,&after)==UMI_STATUS_OK && after.last_sequence==0U && after.text_bytes==0U);
    }else if(strcmp(mode,"push-unlock")==0){
        fail_unlock=1;
        CHECK(UmiAiResponseStreamPush(stream,key,1U,(const unsigned char *)"A",1U,0)==UMI_STATUS_IO_ERROR);
        /* Unlock errors do not undo an already committed event. */
        CHECK(UmiAiResponseStreamInspect(stream,&after)==UMI_STATUS_OK && after.last_sequence==1U && after.text_bytes==1U);
        CHECK(UmiAiResponseStreamPush(stream,key,1U,(const unsigned char *)"A",1U,0)==UMI_STATUS_INVALID_STATE);
    }else if(strcmp(mode,"copy-unlock")==0){
        fail_unlock=1;
        CHECK(UmiAiResponseStreamCopy(stream,&text,&after)==UMI_STATUS_IO_ERROR);
        CHECK(text==sentinel && memcmp(&before,&after,sizeof after)==0);
    }else if(strcmp(mode,"inspect-unlock")==0){
        fail_unlock=1;
        CHECK(UmiAiResponseStreamInspect(stream,&after)==UMI_STATUS_IO_ERROR);
        CHECK(memcmp(&before,&after,sizeof after)==0);
    }else return 2;
    UmiAiResponseStreamDestroy(stream);
    CHECK(failures==1U && live==0U && !tracking_error);
    tracking=0;return 0;
}
