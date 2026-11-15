/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_sse_response.c
 *
 * PURPOSE:
 *   Exercise response status, event media type, framing ambiguity and byte-count admission without network execution.
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
#define END "\r\n"
typedef struct Fixture {const char *name,*headers;UmiStatus status;size_t body;UmiStatus body_status,finish;} Fixture;
static const Fixture fixtures[]={
 {"plain",STATUS TYPE END,UMI_STATUS_OK,5U,UMI_STATUS_OK,UMI_STATUS_OK},
 {"case",STATUS "cOnTeNt-TyPe: TEXT/EVENT-STREAM\r\n" END,UMI_STATUS_OK,0U,UMI_STATUS_OK,UMI_STATUS_OK},
 {"charset",STATUS "Content-Type: text/event-stream; charset=utf-8\r\n" END,UMI_STATUS_OK,0U,UMI_STATUS_OK,UMI_STATUS_OK},
 {"quoted-charset",STATUS "Content-Type: text/event-stream ; charset = \"UTF-8\" \t\r\n" END,UMI_STATUS_OK,0U,UMI_STATUS_OK,UMI_STATUS_OK},
 {"http10","HTTP/1.0 200 OK\r\n" TYPE END,UMI_STATUS_OK,0U,UMI_STATUS_OK,UMI_STATUS_OK},
 {"informational","HTTP/1.1 100 Continue\r\n\r\nHTTP/1.1 103 Early Hints\r\nLink: </example>\r\n\r\n" STATUS TYPE END,UMI_STATUS_OK,0U,UMI_STATUS_OK,UMI_STATUS_OK},
 {"length-exact",STATUS TYPE "Content-Length: 5\r\n" END,UMI_STATUS_OK,5U,UMI_STATUS_OK,UMI_STATUS_OK},
 {"length-short",STATUS TYPE "Content-Length: 5\r\n" END,UMI_STATUS_OK,4U,UMI_STATUS_OK,UMI_STATUS_PARSE_ERROR},
 {"length-long",STATUS TYPE "Content-Length: 4\r\n" END,UMI_STATUS_OK,5U,UMI_STATUS_CAPACITY_EXCEEDED,UMI_STATUS_CAPACITY_EXCEEDED},
 {"chunked",STATUS TYPE "Transfer-Encoding: chunked\r\n" END,UMI_STATUS_OK,5U,UMI_STATUS_OK,UMI_STATUS_OK},
 {"identity",STATUS TYPE "Content-Encoding: identity\r\n" END,UMI_STATUS_OK,5U,UMI_STATUS_OK,UMI_STATUS_OK},
 {"unknown-field",STATUS "X-Example: retained externally\r\n" TYPE END,UMI_STATUS_OK,0U,UMI_STATUS_OK,UMI_STATUS_OK},
 {"missing-type",STATUS END,UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"json-type",STATUS "Content-Type: application/json\r\n" END,UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"wrong-charset",STATUS "Content-Type: text/event-stream;charset=latin1\r\n" END,UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"extra-parameter",STATUS "Content-Type: text/event-stream; charset=utf-8;foo=bar\r\n" END,UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"empty-parameter",STATUS "Content-Type: text/event-stream;\r\n" END,UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"duplicate-type",STATUS TYPE TYPE END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"gzip",STATUS TYPE "Content-Encoding: gzip\r\n" END,UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"duplicate-encoding",STATUS TYPE "Content-Encoding: identity\r\nContent-Encoding: identity\r\n" END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"duplicate-length",STATUS TYPE "Content-Length: 5\r\nContent-Length: 5\r\n" END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"negative-length",STATUS TYPE "Content-Length: -1\r\n" END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"empty-length",STATUS TYPE "Content-Length: \r\n" END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"comma-length",STATUS TYPE "Content-Length: 5,5\r\n" END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"overflow-length",STATUS TYPE "Content-Length: 18446744073709551616\r\n" END,UMI_STATUS_CAPACITY_EXCEEDED,0U,0,0},
 {"length-budget",STATUS TYPE "Content-Length: 33\r\n" END,UMI_STATUS_CAPACITY_EXCEEDED,0U,0,0},
 {"length-chunked",STATUS TYPE "Content-Length: 5\r\nTransfer-Encoding: chunked\r\n" END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"chunked-length",STATUS TYPE "Transfer-Encoding: chunked\r\nContent-Length: 5\r\n" END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"duplicate-transfer",STATUS TYPE "Transfer-Encoding: chunked\r\nTransfer-Encoding: chunked\r\n" END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"unsupported-transfer",STATUS TYPE "Transfer-Encoding: gzip, chunked\r\n" END,UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"http10-chunked","HTTP/1.0 200 OK\r\n" TYPE "Transfer-Encoding: chunked\r\n" END,UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"redirect","HTTP/1.1 302 Found\r\n",UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"busy","HTTP/1.1 503 Unavailable\r\n",UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"rate-limited","HTTP/1.1 429 Limited\r\n",UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"no-content","HTTP/1.1 204 No Content\r\n",UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"upgrade","HTTP/1.1 101 Switching\r\n",UMI_STATUS_UNAVAILABLE,0U,0,0},
 {"http2","HTTP/2 200\r\n",UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"invalid-status","HTTP/1.1 20x OK\r\n",UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"folded-field",STATUS " Content-Type: text/event-stream\r\n" END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"space-before-colon",STATUS "Content-Type : text/event-stream\r\n" END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"missing-colon",STATUS "Content-Type text/event-stream\r\n" END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"control-value",STATUS "X-Bad: \x01\r\n" END,UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"info-length","HTTP/1.1 100 Continue\r\nContent-Length: 0\r\n",UMI_STATUS_PARSE_ERROR,0U,0,0},
 {"second-status",STATUS STATUS,UMI_STATUS_PARSE_ERROR,0U,0,0}
};
static UmiStatus Headers(UmiWebSseResponse *response,const char *headers)
{
    const char *current=headers;
    while(*current){
        const char *end=strstr(current,"\r\n");
        if(end==NULL)return UMI_STATUS_INTERNAL_ERROR;
        size_t bytes=(size_t)(end-current)+2U;
        UmiStatus status=UmiWebSseResponseHeader(response,current,bytes);
        if(status!=UMI_STATUS_OK)return status;
        current+=bytes;
    }
    return UMI_STATUS_OK;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const Fixture *fixture=NULL;
    for(size_t i=0U;i<sizeof fixtures/sizeof fixtures[0];++i)
        if(strcmp(argv[1],fixtures[i].name)==0){fixture=&fixtures[i];break;}
    if(fixture==NULL)return 2;
    UmiWebSseResponseLimits limits={512U,4096U,64U,4U,32U};UmiWebSseResponse *response=NULL;
    CHECK(UmiWebSseResponseCreate(&limits,&response)==UMI_STATUS_OK);
    UmiStatus status=Headers(response,fixture->headers);CHECK(status==fixture->status);
    if(status==UMI_STATUS_OK){
        CHECK(UmiWebSseResponseBody(response,fixture->body)==fixture->body_status);
        CHECK(UmiWebSseResponseFinish(response)==fixture->finish);
        status=fixture->finish;
    }
    UmiWebSseResponseSnapshot snapshot;CHECK(UmiWebSseResponseInspect(response,&snapshot)==UMI_STATUS_OK);
    if(status==UMI_STATUS_OK){
        CHECK(snapshot.state==UMI_WEB_SSE_RESPONSE_COMPLETE && snapshot.failure==UMI_STATUS_OK);
        CHECK(snapshot.http_status==200U && snapshot.body_bytes==fixture->body);
        CHECK(UmiWebSseResponseFinish(response)==UMI_STATUS_OK);
        CHECK(UmiWebSseResponseHeader(response,END,2U)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiWebSseResponseBody(response,1U)==UMI_STATUS_INVALID_STATE);
        if(strcmp(fixture->name,"informational")==0)CHECK(snapshot.informational_responses==2U);
    }else{
        CHECK(snapshot.state==UMI_WEB_SSE_RESPONSE_FAILED && snapshot.failure==status);
        CHECK(UmiWebSseResponseHeader(response,STATUS,strlen(STATUS))==status);
        CHECK(UmiWebSseResponseBody(response,1U)==status && UmiWebSseResponseFinish(response)==status);
    }
    UmiWebSseResponseDestroy(response);return 0;
}
