/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_sse_roundtrip.c
 *
 * PURPOSE:
 *   Exercise formatter-to-parser interoperability and isolate a malformed successor from an owned event.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/sse.h"
#include "umicom/web/sse_parser.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    UmiWebSseLimits limits={4096,4096,64,64,20000};
    UmiWebSseParser *parser=NULL;UmiWebSseMessage *message=NULL;
    CHECK(UmiWebSseParserCreate(&limits,&parser)==UMI_STATUS_OK);
    if(strcmp(argv[1],"successor")==0){
        const unsigned char bytes[]="data:first\n\n\xff";
        size_t consumed=0U;
        CHECK(UmiWebSseParserFeed(parser,bytes,sizeof bytes-1U,&consumed)==UMI_STATUS_OK);
        CHECK(consumed==12U);
        CHECK(UmiWebSseParserTake(parser,&message)==UMI_STATUS_OK);
        size_t second=0U;
        CHECK(UmiWebSseParserFeed(parser,bytes+consumed,sizeof bytes-1U-consumed,&second)==UMI_STATUS_PARSE_ERROR);
        CHECK(second==1U);
        UmiWebSseParserDestroy(parser);
        CHECK(strcmp(message->data,"first")==0);
        UmiWebSseMessageFree(message);return 0;
    }
    if(strcmp(argv[1],"roundtrip")!=0){UmiWebSseParserDestroy(parser);return 2;}
    const char *source[]={"one","a\nb","a\rb","a\r\nb","\n","","\xe2\x82\xac"};
    const char *expected[]={"one","a\nb","a\nb","a\nb","\n","","\xe2\x82\xac"};
    UmiWebSseParserDestroy(parser);
    for(size_t i=0;i<sizeof source/sizeof source[0];++i){
        CHECK(UmiWebSseParserCreate(&limits,&parser)==UMI_STATUS_OK);
        UmiWebSseEvent event={UINT64_MAX,"update",""};
        memcpy(event.data,source[i],strlen(source[i])+1U);
        char wire[16000];size_t length=0U;
        CHECK(umi_web_sse_format(&event,wire,sizeof wire,&length)==UMI_STATUS_OK);
        for(size_t offset=0;offset<length;++offset){
            size_t consumed=0U;
            UmiStatus status=UmiWebSseParserFeed(parser,wire+offset,1U,&consumed);
            CHECK(consumed==1U);
            CHECK(status==(offset+1U==length?UMI_STATUS_OK:UMI_STATUS_BUSY));
        }
        CHECK(UmiWebSseParserTake(parser,&message)==UMI_STATUS_OK);
        CHECK(strcmp(message->event,"update")==0 && strcmp(message->data,expected[i])==0);
        CHECK(strcmp(message->id,"18446744073709551615")==0);
        CHECK(UmiWebSseParserFinish(parser)==UMI_STATUS_OK);
        UmiWebSseMessageFree(message);UmiWebSseParserDestroy(parser);
    }
    return 0;
}
