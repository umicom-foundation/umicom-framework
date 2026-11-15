/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_sse_parser_faults.c
 *
 * PURPOSE:
 *   Inject SSE owner and message allocation failures without losing a ready event.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/sse_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
void *__real_calloc(size_t count,size_t size);
void *__real_malloc(size_t size);
void __real_free(void *pointer);
void *__wrap_calloc(size_t count,size_t size);
void *__wrap_malloc(size_t size);
void __wrap_free(void *pointer);
static int fail_owner,fail_message;
static void *owner_pointer,*message_pointer;
static unsigned owner_frees,message_frees;
void *__wrap_calloc(size_t count,size_t size)
{
    if(fail_owner)return NULL;
    void *result=__real_calloc(count,size);owner_pointer=result;return result;
}
void *__wrap_malloc(size_t size)
{
    if(fail_message)return NULL;
    void *result=__real_malloc(size);message_pointer=result;return result;
}
void __wrap_free(void *pointer)
{
    if(pointer!=NULL && pointer==owner_pointer)++owner_frees;
    if(pointer!=NULL && pointer==message_pointer)++message_frees;
    __real_free(pointer);
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    UmiWebSseLimits limits={64,64,16,16,1024};
    UmiWebSseParser *parser=NULL;
    if(strcmp(argv[1],"owner")==0){
        fail_owner=1;
        CHECK(UmiWebSseParserCreate(&limits,&parser)==UMI_STATUS_OUT_OF_MEMORY && parser==NULL);
        CHECK(owner_frees==0U);return 0;
    }
    if(strcmp(argv[1],"message")!=0)return 2;
    CHECK(UmiWebSseParserCreate(&limits,&parser)==UMI_STATUS_OK);
    size_t consumed=0;CHECK(UmiWebSseParserFeed(parser,"id:7\ndata:x\n\n",13U,&consumed)==UMI_STATUS_OK);
    CHECK(consumed==13U);
    UmiWebSseMessage *message=NULL;fail_message=1;
    CHECK(UmiWebSseParserTake(parser,&message)==UMI_STATUS_OUT_OF_MEMORY && message==NULL);
    UmiWebSseSnapshot snapshot;CHECK(UmiWebSseParserInspect(parser,&snapshot)==UMI_STATUS_OK);
    CHECK(snapshot.state==UMI_WEB_SSE_READY && snapshot.delivered_events==0U);
    fail_message=0;
    CHECK(UmiWebSseParserTake(parser,&message)==UMI_STATUS_OK);
    CHECK(strcmp(message->data,"x")==0 && strcmp(message->id,"7")==0);
    UmiWebSseParserDestroy(parser);
    CHECK(strcmp(message->data,"x")==0);
    UmiWebSseMessageFree(message);
    CHECK(owner_frees==1U && message_frees==1U);
    return 0;
}
