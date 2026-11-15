/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_sse_response_boundaries.c
 *
 * PURPOSE:
 *   Verify bounded header ownership, malformed callback lines and trailer/body ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/sse_response.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
#define STATUS "HTTP/1.1 200 OK\r\n"
#define TYPE "Content-Type: text/event-stream\r\n"
static UmiStatus Header(UmiWebSseResponse *response,const char *line)
{return UmiWebSseResponseHeader(response,line,strlen(line));}
static int Ready(UmiWebSseResponse *response,bool chunked)
{
    CHECK(Header(response,STATUS)==UMI_STATUS_OK);CHECK(Header(response,TYPE)==UMI_STATUS_OK);
    if(chunked)CHECK(Header(response,"Transfer-Encoding: chunked\r\n")==UMI_STATUS_OK);
    CHECK(Header(response,"\r\n")==UMI_STATUS_OK);return 0;
}
static int Arguments(void)
{
    UmiWebSseResponseLimits limits={64U,512U,16U,2U,64U};
    UmiWebSseResponse *sentinel=(UmiWebSseResponse *)(uintptr_t)1U,*response=NULL;
    CHECK(UmiWebSseResponseCreate(NULL,&sentinel)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebSseResponseCreate(&limits,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    for(unsigned i=0U;i<9U;++i){
        UmiWebSseResponseLimits bad=limits;
        switch(i){
        case 0U:bad.line_bytes=1U;break;case 1U:bad.line_bytes=65537U;break;
        case 2U:bad.total_header_bytes=63U;break;case 3U:bad.total_header_bytes=1048577U;break;
        case 4U:bad.header_lines=0U;break;case 5U:bad.header_lines=4097U;break;
        case 6U:bad.informational_responses=17U;break;case 7U:bad.body_bytes=0U;break;
        default:bad.line_bytes=SIZE_MAX;break;
        }
        CHECK(UmiWebSseResponseCreate(&bad,&sentinel)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(sentinel==(UmiWebSseResponse *)(uintptr_t)1U);
    }
    CHECK(UmiWebSseResponseCreate(&limits,&response)==UMI_STATUS_OK);
    CHECK(UmiWebSseResponseHeader(response,NULL,1U)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebSseResponseHeader(response,"",0U)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebSseResponseHeader(NULL,STATUS,strlen(STATUS))==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebSseResponseBody(NULL,1U)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebSseResponseFinish(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    UmiWebSseResponseSnapshot snapshot,old;memset(&snapshot,0xa5,sizeof snapshot);memcpy(&old,&snapshot,sizeof old);
    CHECK(UmiWebSseResponseInspect(NULL,&snapshot)==UMI_STATUS_INVALID_ARGUMENT && memcmp(&snapshot,&old,sizeof old)==0);
    CHECK(UmiWebSseResponseInspect(response,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebSseResponseInspect(response,&snapshot)==UMI_STATUS_OK && snapshot.header_bytes==0U && snapshot.state==UMI_WEB_SSE_RESPONSE_STATUS);
    UmiWebSseResponseDestroy(response);UmiWebSseResponseDestroy(NULL);return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(strcmp(argv[1],"arguments")==0)return Arguments();
    const char *name=argv[1];UmiWebSseResponseLimits limits={64U,512U,16U,2U,64U};
    if(strcmp(name,"line-budget")==0)limits.line_bytes=31U;
    if(strcmp(name,"total-budget")==0)limits.total_header_bytes=64U;
    if(strcmp(name,"line-count")==0)limits.header_lines=2U;
    if(strcmp(name,"info-count")==0)limits.informational_responses=0U;
    if(strcmp(name,"body-budget")==0)limits.body_bytes=5U;
    UmiWebSseResponse *response=NULL;CHECK(UmiWebSseResponseCreate(&limits,&response)==UMI_STATUS_OK);
    UmiStatus expected=UMI_STATUS_PARSE_ERROR,status=UMI_STATUS_OK;
    if(strcmp(name,"empty-transport")==0)status=UmiWebSseResponseFinish(response);
    else if(strcmp(name,"body-before-status")==0)status=UmiWebSseResponseBody(response,1U);
    else if(strcmp(name,"bare-lf")==0)status=Header(response,"HTTP/1.1 200 OK\n");
    else if(strcmp(name,"embedded-cr")==0)status=Header(response,"HTTP/1.1 200\rOK\r\n");
    else if(strcmp(name,"embedded-nul")==0){
        const char input[]="HTTP/1.1 200\0OK\r\n";
        status=UmiWebSseResponseHeader(response,input,sizeof input-1U);
    }else if(strcmp(name,"info-count")==0){
        status=Header(response,"HTTP/1.1 100 Continue\r\n");expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(strcmp(name,"line-budget")==0){
        CHECK(Header(response,STATUS)==UMI_STATUS_OK);status=Header(response,TYPE);expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(strcmp(name,"line-count")==0){
        CHECK(Header(response,STATUS)==UMI_STATUS_OK);CHECK(Header(response,TYPE)==UMI_STATUS_OK);
        status=Header(response,"\r\n");expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(strcmp(name,"total-budget")==0){
        CHECK(Header(response,STATUS)==UMI_STATUS_OK);CHECK(Header(response,TYPE)==UMI_STATUS_OK);
        status=Header(response,"X-Example: 1234567890\r\n");expected=UMI_STATUS_CAPACITY_EXCEEDED;
    }else if(strcmp(name,"body-before-end")==0){
        CHECK(Header(response,STATUS)==UMI_STATUS_OK);CHECK(Header(response,TYPE)==UMI_STATUS_OK);
        status=UmiWebSseResponseBody(response,1U);
    }else if(strcmp(name,"incomplete-headers")==0){
        CHECK(Header(response,STATUS)==UMI_STATUS_OK);status=UmiWebSseResponseFinish(response);
    }else if(strcmp(name,"only-informational")==0){
        CHECK(Header(response,"HTTP/1.1 100 Continue\r\n")==UMI_STATUS_OK);
        CHECK(Header(response,"\r\n")==UMI_STATUS_OK);status=UmiWebSseResponseFinish(response);
    }else if(strcmp(name,"body-budget")==0){
        CHECK(Ready(response,false)==0);CHECK(UmiWebSseResponseBody(response,3U)==UMI_STATUS_OK);
        status=UmiWebSseResponseBody(response,3U);expected=UMI_STATUS_CAPACITY_EXCEEDED;
        UmiWebSseResponseSnapshot snapshot;CHECK(UmiWebSseResponseInspect(response,&snapshot)==UMI_STATUS_OK && snapshot.body_bytes==3U);
    }else if(strcmp(name,"plain-trailer")==0){
        CHECK(Ready(response,false)==0);CHECK(UmiWebSseResponseBody(response,1U)==UMI_STATUS_OK);
        status=Header(response,"X-Result: complete\r\n");
    }else if(strcmp(name,"header-before-body")==0){
        CHECK(Ready(response,true)==0);status=Header(response,"X-Result: complete\r\n");
    }else if(strcmp(name,"zero-body")==0){
        CHECK(Ready(response,false)==0);CHECK(UmiWebSseResponseBody(response,0U)==UMI_STATUS_OK);
        status=UmiWebSseResponseFinish(response);expected=UMI_STATUS_OK;
    }else if(strncmp(name,"trailer-",8U)==0 || strcmp(name,"body-after-trailer")==0 || strcmp(name,"header-after-trailer-end")==0){
        CHECK(Ready(response,true)==0);CHECK(UmiWebSseResponseBody(response,5U)==UMI_STATUS_OK);
        if(strcmp(name,"trailer-content-type")==0)status=Header(response,TYPE);
        else if(strcmp(name,"trailer-length")==0)status=Header(response,"Content-Length: 5\r\n");
        else if(strcmp(name,"trailer-location")==0)status=Header(response,"Location: http://example.invalid\r\n");
        else if(strcmp(name,"trailer-status")==0)status=Header(response,STATUS);
        else{
            CHECK(Header(response,"X-Result: complete\r\n")==UMI_STATUS_OK);
            if(strcmp(name,"body-after-trailer")==0)status=UmiWebSseResponseBody(response,1U);
            else if(strcmp(name,"header-after-trailer-end")==0){
                CHECK(Header(response,"\r\n")==UMI_STATUS_OK);status=Header(response,"X-Late: no\r\n");
            }else if(strcmp(name,"trailer-ended")==0 || strcmp(name,"trailer-unended")==0){
                if(strcmp(name,"trailer-ended")==0)CHECK(Header(response,"\r\n")==UMI_STATUS_OK);
                status=UmiWebSseResponseFinish(response);expected=UMI_STATUS_OK;
            }else return 2;
        }
    }else return 2;
    CHECK(status==expected);
    UmiWebSseResponseSnapshot snapshot;CHECK(UmiWebSseResponseInspect(response,&snapshot)==UMI_STATUS_OK);
    if(expected==UMI_STATUS_OK)CHECK(snapshot.state==UMI_WEB_SSE_RESPONSE_COMPLETE);
    else CHECK(snapshot.state==UMI_WEB_SSE_RESPONSE_FAILED && snapshot.failure==expected);
    UmiWebSseResponseDestroy(response);return 0;
}
