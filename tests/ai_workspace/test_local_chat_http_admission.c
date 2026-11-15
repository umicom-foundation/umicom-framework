/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_chat_http_admission.c
 *
 * PURPOSE:
 *   Verify response admission precedes model text and declared transport length precedes completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_chat_stream_fixture.h"
#include "umicom/web/sse_response.h"
static UmiStatus Header(UmiWebSseResponse *response,const char *line)
{return UmiWebSseResponseHeader(response,line,strlen(line));}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!UmiAiWorkspaceLocalProviderAvailable()){fputs("SKIP: local JSON capability unavailable.\n",stderr);return 77;}
    bool success=strcmp(argv[1],"success")==0,short_body=strcmp(argv[1],"short-body")==0;
    bool wrong_type=strcmp(argv[1],"wrong-type")==0,busy=strcmp(argv[1],"busy")==0;
    if(!success && !short_body && !wrong_type && !busy)return 2;
    AwLocalChatStream *stream=NULL;CHECK(CreateStream(64U,16384U,&stream)==UMI_STATUS_OK);
    UmiWebSseResponseLimits limits={256U,2048U,16U,2U,16384U};UmiWebSseResponse *response=NULL;
    CHECK(UmiWebSseResponseCreate(&limits,&response)==UMI_STATUS_OK);
    UmiStatus status=Header(response,busy?"HTTP/1.1 503 Unavailable\r\n":"HTTP/1.1 200 OK\r\n");
    if(status==UMI_STATUS_OK)status=Header(response,wrong_type?"Content-Type: application/json\r\n":"Content-Type: text/event-stream\r\n");
    const char *wire=TEXT_HELLO STOP DONE;size_t bytes=strlen(wire);
    if(status==UMI_STATUS_OK){
        char length[64];int written=snprintf(length,sizeof length,"Content-Length: %zu\r\n",bytes+(short_body?1U:0U));
        CHECK(written>0 && (size_t)written<sizeof length);
        CHECK(Header(response,length)==UMI_STATUS_OK);CHECK(Header(response,"\r\n")==UMI_STATUS_OK);
        /* Transport callbacks must admit metadata and byte counts before a
         * provider decoder sees data. Never decode rejected error bodies. */
        status=UmiWebSseResponseBody(response,bytes);
        if(status==UMI_STATUS_OK)status=AwLocalChatStreamFeed(stream,wire,bytes);
        if(status==UMI_STATUS_OK)status=UmiWebSseResponseFinish(response);
        if(status==UMI_STATUS_OK)status=AwLocalChatStreamFinish(stream);
    }
    if(success){
        CHECK(status==UMI_STATUS_OK);
        CHECK(CheckSnapshot(stream,AW_LOCAL_CHAT_COMPLETE,UMI_AI_RESPONSE_STREAM_COMPLETE,"Hello",UMI_STATUS_OK)==0);
    }else{
        CHECK(status==(short_body?UMI_STATUS_PARSE_ERROR:UMI_STATUS_UNAVAILABLE));
        CHECK(AwLocalChatStreamFail(stream,status)==status);
        CHECK(CheckSnapshot(stream,AW_LOCAL_CHAT_FAILED,UMI_AI_RESPONSE_STREAM_FAILED,short_body?"Hello":"",status)==0);
    }
    UmiWebSseResponseDestroy(response);AwLocalChatStreamDestroy(stream);return 0;
}
