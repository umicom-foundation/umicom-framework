/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_sse_parser_boundaries.c
 *
 * PURPOSE:
 *   Check SSE budget admission, strict text failures, sticky terminal errors and unchanged output arguments.
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
static int Arguments(void)
{
    UmiWebSseLimits limits={32,32,16,16,100};
    UmiWebSseParser *parser=NULL;
    CHECK(UmiWebSseParserCreate(&limits,&parser)==UMI_STATUS_OK);
    UmiWebSseParser *unchanged=parser;
    CHECK(UmiWebSseParserCreate(NULL,&unchanged)==UMI_STATUS_INVALID_ARGUMENT && unchanged==parser);
    CHECK(UmiWebSseParserCreate(&limits,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    for(size_t which=0;which<10U;++which){
        UmiWebSseLimits bad=limits;
        switch(which){
        case 0:bad.line_bytes=0;break;case 1:bad.data_bytes=0;break;
        case 2:bad.event_bytes=6;break;case 3:bad.id_bytes=0;break;
        case 4:bad.input_bytes=0;break;case 5:bad.line_bytes=SIZE_MAX;break;
        case 6:bad.data_bytes=SIZE_MAX;break;case 7:bad.event_bytes=SIZE_MAX;break;
        case 8:bad.id_bytes=SIZE_MAX;break;default:bad.event_bytes=0;break;
        }
        CHECK(UmiWebSseParserCreate(&bad,&unchanged)==UMI_STATUS_INVALID_ARGUMENT && unchanged==parser);
    }
    size_t consumed=77U;
    CHECK(UmiWebSseParserFeed(NULL,"x",1,&consumed)==UMI_STATUS_INVALID_ARGUMENT && consumed==77U);
    CHECK(UmiWebSseParserFeed(parser,NULL,1,&consumed)==UMI_STATUS_INVALID_ARGUMENT && consumed==77U);
    CHECK(UmiWebSseParserFeed(parser,"x",1,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    UmiWebSseSnapshot snapshot,before;memset(&snapshot,0xa5,sizeof snapshot);memcpy(&before,&snapshot,sizeof before);
    CHECK(UmiWebSseParserInspect(NULL,&snapshot)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&snapshot,&before,sizeof snapshot)==0);
    CHECK(UmiWebSseParserInspect(parser,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    UmiWebSseMessage *message=NULL;
    CHECK(UmiWebSseParserTake(NULL,&message)==UMI_STATUS_INVALID_ARGUMENT && message==NULL);
    CHECK(UmiWebSseParserTake(parser,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebSseParserTake(parser,&message)==UMI_STATUS_BUSY && message==NULL);
    CHECK(UmiWebSseParserFinish(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    UmiWebSseParserDestroy(parser);UmiWebSseParserDestroy(NULL);UmiWebSseMessageFree(NULL);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *mode=argv[1],*wire="data:x\n\n";size_t wire_length=8U;
    UmiWebSseLimits limits={64,64,16,16,1024};
    UmiStatus wanted=UMI_STATUS_CAPACITY_EXCEEDED;int finish=0;
    if(strcmp(mode,"arguments")==0)return Arguments();
    if(strcmp(mode,"line-limit")==0)limits.line_bytes=5;
    else if(strcmp(mode,"data-limit")==0){limits.data_bytes=2;wire="data:a\ndata:b\n\n";wire_length=strlen(wire);}
    else if(strcmp(mode,"event-limit")==0){limits.event_bytes=7;wire="event: overflow\n";wire_length=strlen(wire);}
    else if(strcmp(mode,"id-limit")==0){limits.id_bytes=3;wire="id: abcd\n";wire_length=strlen(wire);}
    else if(strcmp(mode,"input-limit")==0)limits.input_bytes=1;
    else if(strcmp(mode,"exact-limits")==0){limits=(UmiWebSseLimits){6,1,7,1,8};wanted=UMI_STATUS_OK;}
    else if(strcmp(mode,"invalid-leading")==0){wire="\xff";wire_length=1;wanted=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"invalid-continuation")==0){wire="\xe2x";wire_length=2;wanted=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"overlong")==0){wire="\xc0\x80";wire_length=2;wanted=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"surrogate")==0){wire="\xed\xa0\x80";wire_length=3;wanted=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"out-of-range")==0){wire="\xf4\x90\x80\x80";wire_length=4;wanted=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"nul-data")==0){wire="data:\0\n\n";wire_length=8;wanted=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"nul-comment")==0){wire=":\0\n";wire_length=3;wanted=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"eof-utf8")==0){wire="data:\xe2\x82";wire_length=7;wanted=UMI_STATUS_BUSY;finish=1;}
    else if(strcmp(mode,"empty-feed")==0){wire=NULL;wire_length=0;wanted=UMI_STATUS_BUSY;}
    else if(strcmp(mode,"sticky-failure")==0){wire="data:accepted\n\xff";wire_length=strlen(wire);wanted=UMI_STATUS_PARSE_ERROR;}
    else return 2;
    UmiWebSseParser *parser=NULL;CHECK(UmiWebSseParserCreate(&limits,&parser)==UMI_STATUS_OK);
    size_t consumed=777U;
    CHECK(UmiWebSseParserFeed(parser,wire,wire_length,&consumed)==wanted);
    CHECK(consumed<=wire_length);
    if(strcmp(mode,"input-limit")==0)CHECK(consumed==1U);
    if(finish){CHECK(UmiWebSseParserFinish(parser)==UMI_STATUS_PARSE_ERROR);wanted=UMI_STATUS_PARSE_ERROR;}
    if(wanted==UMI_STATUS_OK){
        CHECK(consumed==wire_length);
        UmiWebSseMessage *message=NULL;
        CHECK(UmiWebSseParserTake(parser,&message)==UMI_STATUS_OK);
        CHECK(strcmp(message->data,"x")==0);UmiWebSseMessageFree(message);
        CHECK(UmiWebSseParserFinish(parser)==UMI_STATUS_OK);
    }else if(wanted==UMI_STATUS_BUSY){
        CHECK(consumed==0U && UmiWebSseParserFinish(parser)==UMI_STATUS_OK);
    }else{
        UmiWebSseSnapshot before,after;
        CHECK(UmiWebSseParserInspect(parser,&before)==UMI_STATUS_OK);
        CHECK(before.state==UMI_WEB_SSE_FAILED && before.failure==wanted);
        CHECK(before.input_bytes==consumed);
        consumed=777U;
        CHECK(UmiWebSseParserFeed(parser,"data:late\n\n",11U,&consumed)==wanted && consumed==0U);
        CHECK(UmiWebSseParserFinish(parser)==wanted);
        UmiWebSseMessage *message=NULL;
        CHECK(UmiWebSseParserTake(parser,&message)==wanted && message==NULL);
        CHECK(UmiWebSseParserInspect(parser,&after)==UMI_STATUS_OK);
        CHECK(after.input_bytes==before.input_bytes && after.delivered_events==0U);
        CHECK(after.pending_data_bytes==before.pending_data_bytes);
        if(strcmp(mode,"sticky-failure")==0)CHECK(after.pending_data_bytes==8U);
    }
    UmiWebSseParserDestroy(parser);return 0;
}
