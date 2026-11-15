/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_sse_format.c
 *
 * PURPOSE:
 *   Check complete SSE records, multiline data isolation and failure-preserving output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/sse.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
static size_t Fields(const char *text)
{
    size_t count=0U;
    for(const char *p=text;(p=strstr(p,"data: "))!=NULL;p+=6)++count;
    return count;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *mode=argv[1],*data="ready",*tail="data: ready\n\n";
    UmiWebSseEvent event={7U,"update","ready"};
    char output[16000],before[16000],expected[16000];size_t length=777U;
    UmiStatus wanted=UMI_STATUS_OK;
    if(strcmp(mode,"empty")==0){data="";tail="data: \n\n";}
    else if(strcmp(mode,"lf")==0){data="first\nsecond";tail="data: first\ndata: second\n\n";}
    else if(strcmp(mode,"cr")==0){data="first\rsecond";tail="data: first\ndata: second\n\n";}
    else if(strcmp(mode,"crlf")==0){data="first\r\nsecond";tail="data: first\ndata: second\n\n";}
    else if(strcmp(mode,"trailing-lf")==0){data="first\n";tail="data: first\ndata: \n\n";}
    else if(strcmp(mode,"trailing-crlf")==0){data="first\r\n";tail="data: first\ndata: \n\n";}
    else if(strcmp(mode,"leading-lf")==0){data="\nsecond";tail="data: \ndata: second\n\n";}
    else if(strcmp(mode,"consecutive")==0){data="\n\n";tail="data: \ndata: \ndata: \n\n";}
    else if(strcmp(mode,"colon")==0){data=":comment\nid: forged\n\nevent: injected";tail="data: :comment\ndata: id: forged\ndata: \ndata: event: injected\n\n";}
    else if(strcmp(mode,"unicode")==0){data="\xe2\x82\xac\n\xc3\xa9";tail="data: \xe2\x82\xac\ndata: \xc3\xa9\n\n";}
    else if(strcmp(mode,"id-max")==0)event.id=UINT64_MAX;
    memcpy(event.data,data,strlen(data)+1U);
    if(strcmp(mode,"unterminated-event")==0){memset(event.event,'x',sizeof event.event);wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"unterminated-data")==0){memset(event.data,'x',sizeof event.data);wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"event-lf")==0){memcpy(event.event,"x\ndata: forged",15U);wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"event-cr")==0){memcpy(event.event,"x\rid: forged",13U);wanted=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"invalid-event-utf8")==0){event.event[0]=(char)0xff;event.event[1]='\0';wanted=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"invalid-data-utf8")==0){event.data[0]=(char)0xc0;event.data[1]=(char)0x80;event.data[2]='\0';wanted=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"null-event")==0 || strcmp(mode,"null-output")==0)wanted=UMI_STATUS_INVALID_ARGUMENT;
    else if(strcmp(mode,"all-newlines")==0){memset(event.data,'\n',sizeof event.data-1U);event.data[sizeof event.data-1U]='\0';}
    else if(strcmp(mode,"max-fields")==0){
        memset(event.event,'e',sizeof event.event-1U);event.event[sizeof event.event-1U]='\0';
        memset(event.data,'d',sizeof event.data-1U);event.data[sizeof event.data-1U]='\0';
    }
    if(strcmp(mode,"id-max")==0)
        (void)snprintf(expected,sizeof expected,"id: 18446744073709551615\nevent: update\n%s",tail);
    else (void)snprintf(expected,sizeof expected,"id: 7\nevent: update\n%s",tail);
    memset(output,0xa5,sizeof output);memcpy(before,output,sizeof before);
    if(strcmp(mode,"capacity-sweep")==0){
        for(size_t capacity=0U;capacity<=strlen(expected);++capacity){
            CHECK(umi_web_sse_format(&event,output,capacity,&length)==UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(length==777U && memcmp(output,before,sizeof output)==0);
        }
        CHECK(umi_web_sse_format(&event,output,strlen(expected)+1U,&length)==UMI_STATUS_OK);
        CHECK(length==strlen(expected) && strcmp(output,expected)==0);return 0;
    }
    if(strcmp(mode,"alias-data")==0){
        CHECK(umi_web_sse_format(&event,event.data,sizeof event.data,&length)==UMI_STATUS_OK);
        CHECK(strcmp(event.data,expected)==0 && length==strlen(expected));return 0;
    }
    if(strcmp(mode,"alias-record")==0){
        CHECK(umi_web_sse_format(&event,(char *)&event,sizeof event,&length)==UMI_STATUS_OK);
        CHECK(strcmp((const char *)&event,expected)==0 && length==strlen(expected));return 0;
    }
    CHECK(umi_web_sse_format(strcmp(mode,"null-event")==0?NULL:&event,
        strcmp(mode,"null-output")==0?NULL:output,sizeof output,
        strcmp(mode,"optional-length")==0?NULL:&length)==wanted);
    if(wanted!=UMI_STATUS_OK)CHECK(length==777U && memcmp(output,before,sizeof output)==0);
    else if(strcmp(mode,"all-newlines")==0){
        CHECK(Fields(output)==sizeof event.data);
        CHECK(length==20U+sizeof event.data*7U+1U && strlen(output)==length);
    }else if(strcmp(mode,"max-fields")==0){
        CHECK(Fields(output)==1U && length==2132U && strlen(output)==length);
        CHECK(memcmp(output+13U,event.event,63U)==0);
        CHECK(strstr(output,"\ndata: ")!=NULL);
    }else{
        CHECK(strcmp(output,expected)==0);
        CHECK(strcmp(mode,"optional-length")==0?length==777U:length==strlen(expected));
    }
    return 0;
}
