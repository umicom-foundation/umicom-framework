/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_sse_parser.c
 *
 * PURPOSE:
 *   Verify SSE receiving semantics under every two-chunk split and independent copied-message ownership.
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
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
typedef struct Expected {
    const char *event,*data,*id;
    int has_retry;uint64_t retry;
} Expected;
typedef struct Fixture {
    const char *name,*wire;
    size_t count;
    Expected events[3];
} Fixture;
static const Fixture fixtures[]={
    {"basic","data: hello\n\n",1,{{"message","hello","",0,0}}},
    {"empty-data","data:\n\n",1,{{"message","","",0,0}}},
    {"multiline","data: one\ndata: two\n\n",1,{{"message","one\ntwo","",0,0}}},
    {"trailing-data","data: one\ndata:\n\n",1,{{"message","one\n","",0,0}}},
    {"comments",": keepalive\n\n:later\ndata:x\n\n",1,{{"message","x","",0,0}}},
    {"unknown","other: ignored\ndata:x\n\n",1,{{"message","x","",0,0}}},
    {"case-sensitive","Data: ignored\nEVENT: ignored\ndata:x\n\n",1,{{"message","x","",0,0}}},
    {"no-colon","data\n\n",1,{{"message","","",0,0}}},
    {"one-space","data:  retained\n\n",1,{{"message"," retained","",0,0}}},
    {"tab-value","data:\tx\n\n",1,{{"message","\tx","",0,0}}},
    {"event-name","event: update\ndata:x\n\n",1,{{"update","x","",0,0}}},
    {"empty-name","event: update\nevent:\ndata:x\n\n",1,{{"message","x","",0,0}}},
    {"metadata-block","event: ignored\nid: abc\n\ndata:x\n\n",1,{{"message","x","abc",0,0}}},
    {"id-persistence","id: abc\ndata:x\n\ndata:y\n\n",2,{{"message","x","abc",0,0},{"message","y","abc",0,0}}},
    {"id-reset","id: abc\ndata:x\n\nid\ndata:y\n\n",2,{{"message","x","abc",0,0},{"message","y","",0,0}}},
    {"retry","retry: 0012\ndata:x\n\ndata:y\n\n",2,{{"message","x","",1,12},{"message","y","",1,12}}},
    {"retry-invalid","retry: 8\nretry: +4\nretry: 3x\nretry:\ndata:x\n\n",1,{{"message","x","",1,8}}},
    {"retry-overflow","retry: 8\nretry: 18446744073709551616\ndata:x\n\n",1,{{"message","x","",1,8}}},
    {"retry-maximum","retry: 18446744073709551615\ndata:x\n\n",1,{{"message","x","",1,UINT64_MAX}}},
    {"cr","data:x\r\rdata:y\r\r",2,{{"message","x","",0,0},{"message","y","",0,0}}},
    {"crlf","data:x\r\n\r\ndata:y\r\n\r\n",2,{{"message","x","",0,0},{"message","y","",0,0}}},
    {"mixed-newlines","data:x\rdata:y\n\r\n",1,{{"message","x\ny","",0,0}}},
    {"leading-bom","\xef\xbb\xbf""data:x\n\n",1,{{"message","x","",0,0}}},
    {"later-bom","data:\xef\xbb\xbf""x\n\n",1,{{"message","\xef\xbb\xbf""x","",0,0}}},
    {"utf8","event: \xc3\xa9\nid:\xe2\x82\xac\ndata:\xf0\x9f\x8c\x8d\n\n",1,{{"\xc3\xa9","\xf0\x9f\x8c\x8d","\xe2\x82\xac",0,0}}},
    {"unfinished-line","data: x",0,{{0}}},
    {"unfinished-event","data: x\n",0,{{0}}},
    {"metadata-only","id: abc\nretry: 0\nevent: unused\n\n",0,{{0}}},
    {"coalesced","data:a\n\ndata:b\n\ndata:c\n\n",3,{{"message","a","",0,0},{"message","b","",0,0},{"message","c","",0,0}}}
};
static int Match(const UmiWebSseMessage *actual,const Expected *expected)
{
    CHECK(strcmp(actual->event,expected->event)==0);
    CHECK(strcmp(actual->data,expected->data)==0);
    CHECK(strcmp(actual->id,expected->id)==0);
    CHECK(actual->event_bytes==strlen(expected->event));
    CHECK(actual->data_bytes==strlen(expected->data));
    CHECK(actual->id_bytes==strlen(expected->id));
    CHECK(actual->has_retry==expected->has_retry && actual->retry_milliseconds==expected->retry);
    return 0;
}
static int Run(const Fixture *fixture,size_t split,int one_byte)
{
    UmiWebSseLimits limits={256,256,64,64,4096};
    UmiWebSseParser *parser=NULL;UmiWebSseMessage *messages[3]={NULL,NULL,NULL};
    CHECK(UmiWebSseParserCreate(&limits,&parser)==UMI_STATUS_OK);
    size_t length=strlen(fixture->wire),offset=0U,count=0U;
    while(offset<length){
        size_t end=one_byte?offset+1U:(offset<split?split:length),consumed=999U;
        UmiStatus status=UmiWebSseParserFeed(parser,fixture->wire+offset,end-offset,&consumed);
        CHECK(consumed>0U && consumed<=end-offset);offset+=consumed;
        if(status==UMI_STATUS_OK){
            UmiWebSseSnapshot ready;CHECK(UmiWebSseParserInspect(parser,&ready)==UMI_STATUS_OK);
            CHECK(ready.state==UMI_WEB_SSE_READY);
            size_t again=999U;
            CHECK(UmiWebSseParserFeed(parser,"ignored",7U,&again)==UMI_STATUS_OK && again==0U);
            CHECK(UmiWebSseParserFinish(parser)==UMI_STATUS_BUSY);
            CHECK(count<fixture->count);
            CHECK(UmiWebSseParserTake(parser,&messages[count])==UMI_STATUS_OK);
            CHECK(Match(messages[count],&fixture->events[count])==0);++count;
        }else CHECK(status==UMI_STATUS_BUSY && offset==end);
    }
    CHECK(count==fixture->count);
    CHECK(UmiWebSseParserFinish(parser)==UMI_STATUS_OK);
    CHECK(UmiWebSseParserFinish(parser)==UMI_STATUS_OK);
    UmiWebSseSnapshot snapshot;CHECK(UmiWebSseParserInspect(parser,&snapshot)==UMI_STATUS_OK);
    CHECK(snapshot.state==UMI_WEB_SSE_FINISHED && snapshot.failure==UMI_STATUS_OK);
    CHECK(snapshot.input_bytes==length && snapshot.delivered_events==count);
    CHECK(snapshot.pending_line_bytes==0U && snapshot.pending_data_bytes==0U);
    size_t consumed=999U;
    CHECK(UmiWebSseParserFeed(parser,NULL,0,&consumed)==UMI_STATUS_INVALID_STATE && consumed==0U);
    UmiWebSseMessage *missing=NULL;
    CHECK(UmiWebSseParserTake(parser,&missing)==UMI_STATUS_INVALID_STATE && missing==NULL);
    UmiWebSseParserDestroy(parser);
    for(size_t i=0U;i<count;++i){
        CHECK(Match(messages[i],&fixture->events[i])==0);
        UmiWebSseMessageFree(messages[i]);
    }
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    for(size_t i=0U;i<sizeof fixtures/sizeof fixtures[0];++i){
        if(strcmp(argv[1],fixtures[i].name)!=0)continue;
        /* Expected messages are fixed protocol examples, independent of the
         * parser. Every byte boundary can separate two transport callbacks. */
        for(size_t split=0U;split<=strlen(fixtures[i].wire);++split)
            CHECK(Run(&fixtures[i],split,0)==0);
        CHECK(Run(&fixtures[i],0U,1)==0);
        return 0;
    }
    return 2;
}
