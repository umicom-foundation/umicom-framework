/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_http_stream.c
 *
 * PURPOSE:
 *   Exercise the actual stream callbacks with admitted headers, cancellation and injected transport outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_http_stream_internal.h"
#include "local_http_internal.h"
#include "local_chat_stream_fixture.h"
static AwLocalHttpCallbacks selected;
static const char *scenario;
static UmiAiWorkspaceCancellation *token;
static size_t performs,configurations;
static UmiStatus Configure(CURL *curl,uint16_t port,uint32_t timeout,const char *body,
    struct curl_slist *headers,const AwLocalHttpCallbacks *callbacks,bool streaming);
static CURLcode Perform(CURL *curl);
/* Only transport setup/execution is intercepted. Real header admission,
 * callbacks, model sequencing, cancellation and text owners run unchanged.
 * libcurl easy handles and header lists still have real allocation/cleanup. */
#define AwLocalHttpConfigure Configure
#ifdef curl_easy_perform
#undef curl_easy_perform
#endif
#define curl_easy_perform Perform
#include "../../src/ai_workspace/local_http_stream.c"
#undef curl_easy_perform
#undef AwLocalHttpConfigure
static UmiStatus Configure(CURL *curl,uint16_t port,uint32_t timeout,const char *body,
    struct curl_slist *headers,const AwLocalHttpCallbacks *callbacks,bool streaming)
{
    ++configurations;
    if(curl==NULL || port!=8080U || timeout!=1000U || body==NULL || headers==NULL || !streaming)
        return UMI_STATUS_INVALID_ARGUMENT;
    selected=*callbacks;
    return strcmp(scenario,"setup-policy")==0?UMI_STATUS_UNAVAILABLE:UMI_STATUS_OK;
}
static bool HeaderLine(const char *line)
{return selected.header((char *)line,1U,strlen(line),selected.header_context)==strlen(line);}
static bool Payload(const char *body)
{return selected.write((char *)body,1U,strlen(body),selected.write_context)==strlen(body);}
static CURLcode Perform(CURL *curl)
{
    (void)curl;++performs;
    if(strcmp(scenario,"timeout-before")==0)return CURLE_OPERATION_TIMEDOUT;
    if(strcmp(scenario,"header-overflow")==0){
        if(selected.header("",SIZE_MAX,2U,selected.header_context)!=0U)return CURLE_OK;
        return CURLE_WRITE_ERROR;
    }
    if(!HeaderLine(strcmp(scenario,"status-busy")==0?"HTTP/1.1 503 Unavailable\r\n":"HTTP/1.1 200 OK\r\n"))return CURLE_WRITE_ERROR;
    if(strcmp(scenario,"missing-type")!=0 &&
        !HeaderLine(strcmp(scenario,"wrong-type")==0?"Content-Type: application/json\r\n":"Content-Type: text/event-stream\r\n"))return CURLE_WRITE_ERROR;
    if(strcmp(scenario,"short-length")==0){
        char line[64];(void)snprintf(line,sizeof line,"Content-Length: %zu\r\n",strlen(TEXT_HELLO STOP DONE)+1U);
        if(!HeaderLine(line))return CURLE_WRITE_ERROR;
    }
    if(strcmp(scenario,"bad-trailer")==0 && !HeaderLine("Transfer-Encoding: chunked\r\n"))return CURLE_WRITE_ERROR;
    if(!HeaderLine("\r\n"))return CURLE_WRITE_ERROR;
    if(strcmp(scenario,"write-overflow")==0){
        if(selected.write("",SIZE_MAX,2U,selected.write_context)!=0U)return CURLE_OK;
        return CURLE_WRITE_ERROR;
    }
    if(strcmp(scenario,"body-limit")==0){
        if(!Payload(TEXT_HELLO STOP DONE))return CURLE_WRITE_ERROR;
        return CURLE_OK;
    }
    if(strcmp(scenario,"fragmented")==0){
        const char *wire=TEXT_HELLO CHUNK("local-1","test-model","{\"content\":\" €\"}","null","") STOP DONE;
        for(size_t i=0U;i<strlen(wire);++i)
            if(selected.write((char *)wire+i,1U,1U,selected.write_context)!=1U)return CURLE_WRITE_ERROR;
        return CURLE_OK;
    }
    if(!Payload(TEXT_HELLO))return CURLE_WRITE_ERROR;
    if(strcmp(scenario,"timeout-partial")==0)return CURLE_OPERATION_TIMEDOUT;
    if(strcmp(scenario,"cancel-progress")==0){
        UmiAiWorkspaceCancellationRequest(token);
        return selected.progress(selected.progress_context,0,0,0,0)?CURLE_ABORTED_BY_CALLBACK:CURLE_OK;
    }
    if(strcmp(scenario,"bad-json")==0)return Payload("data: broken\n\n")?CURLE_OK:CURLE_WRITE_ERROR;
    if(strcmp(scenario,"early-done")==0)return Payload(DONE)?CURLE_OK:CURLE_WRITE_ERROR;
    if(!Payload(STOP))return CURLE_WRITE_ERROR;
    if(strcmp(scenario,"missing-done")==0)return CURLE_OK;
    if(!Payload(DONE))return CURLE_WRITE_ERROR;
    if(strcmp(scenario,"duplicate-done")==0)return Payload(DONE)?CURLE_OK:CURLE_WRITE_ERROR;
    if(strcmp(scenario,"bad-trailer")==0)return HeaderLine("Content-Type: text/event-stream\r\n")?CURLE_OK:CURLE_WRITE_ERROR;
    if(strcmp(scenario,"truncated-http")==0)return CURLE_PARTIAL_FILE;
    if(strcmp(scenario,"cancel-after-done")==0)UmiAiWorkspaceCancellationRequest(token);
    return CURLE_OK;
}
static int Preconditions(AwLocalChatStream *stream)
{
    AwLocalHttpStreamResult result,before;memset(&result,0xa5,sizeof result);memcpy(&before,&result,sizeof before);
    CHECK(AwLocalChatExchangeStream(1023U,1000U,"{}",16384U,token,stream,&result)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatExchangeStream(8080U,99U,"{}",16384U,token,stream,&result)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatExchangeStream(8080U,60001U,"{}",16384U,token,stream,&result)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatExchangeStream(8080U,1000U,NULL,16384U,token,stream,&result)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatExchangeStream(8080U,1000U,"{}",0U,token,stream,&result)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatExchangeStream(8080U,1000U,"{}",16384U,token,NULL,&result)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatExchangeStream(8080U,1000U,"{}",16384U,token,stream,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalChatStreamFeed(stream,TEXT_HELLO,strlen(TEXT_HELLO))==UMI_STATUS_OK);
    CHECK(AwLocalChatExchangeStream(8080U,1000U,"{}",16384U,token,stream,&result)==UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&result,&before,sizeof result)==0 && performs==0U && configurations==0U);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!UmiAiWorkspaceLocalProviderAvailable()){fputs("SKIP: native HTTP/JSON capability unavailable.\n",stderr);return 77;}
    scenario=argv[1];
    if(curl_global_init(CURL_GLOBAL_DEFAULT)!=CURLE_OK)return 1;
    CHECK(UmiAiWorkspaceCancellationCreate(&token)==UMI_STATUS_OK);
    AwLocalChatStream *stream=NULL;CHECK(CreateStream(64U,16384U,&stream)==UMI_STATUS_OK);
    if(strcmp(scenario,"preconditions")==0){
        int status=Preconditions(stream);AwLocalChatStreamDestroy(stream);
        UmiAiWorkspaceCancellationDestroy(token);curl_global_cleanup();return status;
    }
    UmiStatus expected=UMI_STATUS_OK;const char *text="Hello";
    if(strcmp(scenario,"fragmented")==0)text="Hello €";
    else if(strcmp(scenario,"wrong-type")==0 || strcmp(scenario,"missing-type")==0 ||
        strcmp(scenario,"status-busy")==0 || strcmp(scenario,"setup-policy")==0){expected=UMI_STATUS_UNAVAILABLE;text="";}
    else if(strcmp(scenario,"timeout-before")==0){expected=UMI_STATUS_TIMEOUT;text="";}
    else if(strcmp(scenario,"timeout-partial")==0)expected=UMI_STATUS_TIMEOUT;
    else if(strcmp(scenario,"truncated-http")==0)expected=UMI_STATUS_IO_ERROR;
    else if(strcmp(scenario,"cancel-before")==0){expected=UMI_STATUS_CANCELLED;text="";UmiAiWorkspaceCancellationRequest(token);}
    else if(strcmp(scenario,"cancel-progress")==0 || strcmp(scenario,"cancel-after-done")==0)expected=UMI_STATUS_CANCELLED;
    else if(strcmp(scenario,"missing-done")==0 || strcmp(scenario,"early-done")==0 ||
        strcmp(scenario,"duplicate-done")==0 || strcmp(scenario,"bad-json")==0 ||
        strcmp(scenario,"short-length")==0 || strcmp(scenario,"bad-trailer")==0)expected=UMI_STATUS_PARSE_ERROR;
    else if(strcmp(scenario,"body-limit")==0 || strcmp(scenario,"header-overflow")==0 ||
        strcmp(scenario,"write-overflow")==0){expected=UMI_STATUS_CAPACITY_EXCEEDED;text="";}
    else if(strcmp(scenario,"success")!=0)return 2;
    AwLocalHttpStreamResult result;
    UmiStatus status=AwLocalChatExchangeStream(8080U,1000U,"{\"stream\":true}",
        strcmp(scenario,"body-limit")==0?16U:16384U,token,stream,&result);
    CHECK(status==expected && result.status==expected);
    CHECK(CheckSnapshot(stream,expected==UMI_STATUS_OK?AW_LOCAL_CHAT_COMPLETE:
        expected==UMI_STATUS_CANCELLED?AW_LOCAL_CHAT_CANCELLED:AW_LOCAL_CHAT_FAILED,
        expected==UMI_STATUS_OK?UMI_AI_RESPONSE_STREAM_COMPLETE:
        expected==UMI_STATUS_CANCELLED?UMI_AI_RESPONSE_STREAM_CANCELLED:UMI_AI_RESPONSE_STREAM_FAILED,
        text,expected)==0);
    if(strcmp(scenario,"status-busy")==0)CHECK(result.response.http_status==503U);
    if(strcmp(scenario,"cancel-before")==0)CHECK(performs==0U && configurations==0U);
    else if(strcmp(scenario,"setup-policy")==0)CHECK(performs==0U && configurations==1U);
    else CHECK(performs==1U && configurations==1U);
    AwLocalChatStreamDestroy(stream);UmiAiWorkspaceCancellationDestroy(token);curl_global_cleanup();return 0;
}
