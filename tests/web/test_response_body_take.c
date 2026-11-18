/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_response_body_take.c
 *
 * PURPOSE:
 *   Verify transferred pointer identity, matching release and failure ownership without a payload copy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/web/response_body.h"
static unsigned live, fail_next, released;
static size_t allocation_size;
/* Compare identity while storage is alive; retain no freed pointer for later use. */
static void *expected_release;
static int release_matched;
static void *TestAllocate(size_t size)
{
    allocation_size=size;
    if(fail_next) { fail_next=0; return NULL; }
    void *memory=malloc(size);
    if(memory!=NULL)++live;
    return memory;
}
static void TestFree(void *memory)
{
    if(memory!=NULL)--live;
    free(memory);
}
/* Compile the real production implementation with allocation failure injection.
 * Only this translation unit redirects its allocations; no process-wide hooks. */
#define malloc TestAllocate
#define free TestFree
#include "../../src/web/response_body.c"
#undef malloc
#undef free
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;} } while(0)
static void Release(void *context, void *bytes)
{
    unsigned *count=context;
    ++*count;
/* Compare the release identity before freeing; the retained comparison below previously read dangling pointer values. The former implementation is retained for owner review. */
#if 0
    last_released=bytes;
#endif
    release_matched=(bytes==expected_release);
    free(bytes);
}
int main(void)
{
    unsigned char *data=malloc(4096);
    CHECK(data!=NULL);
    memset(data,0x4a,4096);
    data[23]=0;
    unsigned char *original=data;
    UmiWebResponseBody *body=NULL;
    CHECK(UmiWebResponseBodyTake(&data,4096,Release,&released,&body)==UMI_STATUS_OK);
    CHECK(data==NULL && released==0 && allocation_size<4096 && live==1);
    CHECK(UmiWebResponseBodyBytes(body)==original && UmiWebResponseBodyLength(body)==4096);
    CHECK(((const unsigned char *)UmiWebResponseBodyBytes(body))[23]==0);
    expected_release=original;
    UmiWebResponseBodyDestroy(body);
#if 0
    /* Retained for review: comparing freed pointer values is not valid evidence. */
    CHECK(released==1 && last_released==original && live==0);
#endif
    CHECK(released==1 && release_matched && live==0);
    UmiWebResponseBodyDestroy(NULL);
    CHECK(released==1);

    data=malloc(8);CHECK(data!=NULL);original=data;
    body=(UmiWebResponseBody *)(void *)data;
    CHECK(UmiWebResponseBodyTake(&data,8,NULL,&released,&body)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(data==original && body==NULL && released==1 && live==0);
    CHECK(UmiWebResponseBodyTake(&data,UMI_WEB_RESPONSE_BODY_LIMIT+1U,Release,&released,&body)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(data==original && body==NULL && released==1);
    CHECK(UmiWebResponseBodyTake(NULL,0,Release,&released,&body)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebResponseBodyTake(&data,8,Release,&released,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(data==original && released==1);
    fail_next=1;
    CHECK(UmiWebResponseBodyTake(&data,8,Release,&released,&body)==UMI_STATUS_OUT_OF_MEMORY);
    CHECK(data==original && body==NULL && released==1 && live==0);
    free(data);

    data=NULL;
    expected_release=NULL;
    CHECK(UmiWebResponseBodyTake(&data,1,Release,&released,&body)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebResponseBodyTake(&data,0,Release,&released,&body)==UMI_STATUS_OK);
    CHECK(UmiWebResponseBodyLength(body)==0 && UmiWebResponseBodyBytes(body)==NULL);
    UmiWebResponseBodyDestroy(body);
    CHECK(released==2 && release_matched && live==0);

    const unsigned char copied[]={1,0,3};
    CHECK(UmiWebResponseBodyCreate(copied,sizeof copied,&body)==UMI_STATUS_OK);
    CHECK(UmiWebResponseBodyBytes(body)!=copied && memcmp(UmiWebResponseBodyBytes(body),copied,sizeof copied)==0);
    UmiWebResponseBodyDestroy(body);
    CHECK(released==2 && live==0);
    fail_next=1;
    CHECK(UmiWebResponseBodyCreate(copied,sizeof copied,&body)==UMI_STATUS_OUT_OF_MEMORY);
    CHECK(body==NULL && live==0);
    return 0;
}
