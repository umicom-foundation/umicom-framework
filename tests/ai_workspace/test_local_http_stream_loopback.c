/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_http_stream_loopback.c
 *
 * PURPOSE:
 *   Exercise native streaming HTTP with a joined loopback peer and real encoded requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "local_chat_internal.h"
#include "local_http_stream_internal.h"
#include "local_http_peer.h"
#include "local_chat_stream_fixture.h"
#include <curl/curl.h>
#include <json-c/json.h>

static int PoisonProxyEnvironment(void)
{
    /* Each case is a separate test process; no parent environment is modified. */
    const char *names[]={"http_proxy","HTTP_PROXY","all_proxy","ALL_PROXY","no_proxy","NO_PROXY"};
    for(size_t i=0U;i<sizeof names/sizeof names[0];++i){
        const char *value=i<4U?"http://127.0.0.1:1":"";
#ifdef _WIN32
        if(_putenv_s(names[i],value)!=0)return 1;
#else
        if(setenv(names[i],value,1)!=0)return 1;
#endif
    }
    return 0;
}
static int CheckRequest(const UmiWebRequest *request,const char *encoded)
{
    CHECK(request->method==UMI_HTTP_METHOD_POST);
    CHECK(strcmp(request->path,"/v1/chat/completions")==0 && request->query[0]=='\0');
    CHECK(strcmp(request->version,"HTTP/1.1")==0);
    const char *accept=umi_web_request_header(request,"Accept");
    const char *type=umi_web_request_header(request,"Content-Type");
    CHECK(accept!=NULL && strcmp(accept,"text/event-stream")==0);
    CHECK(type!=NULL && strcmp(type,"application/json")==0);
    CHECK(request->body_length==strlen(encoded));
    CHECK(memcmp(request->body,encoded,request->body_length)==0);
    json_object *body=json_tokener_parse(request->body),*field=NULL;
    CHECK(body!=NULL);
    CHECK(json_object_object_get_ex(body,"stream",&field) && json_object_get_boolean(field));
    CHECK(json_object_object_get_ex(body,"model",&field) && strcmp(json_object_get_string(field),"test-model")==0);
    json_object_put(body);return 0;
}
static int CheckResult(AwLocalChatStream *stream,const AwLocalHttpStreamResult *result,
    UmiStatus status,UmiStatus expected,const char *text)
{
    if(status!=expected || result->status!=expected){
        fprintf(stderr,"exchange status %d; result %d; expected %d; HTTP %u\n",
            (int)status,(int)result->status,(int)expected,(unsigned)result->response.http_status);
        return 1;
    }
    return CheckSnapshot(stream,expected==UMI_STATUS_OK?AW_LOCAL_CHAT_COMPLETE:
        expected==UMI_STATUS_CANCELLED?AW_LOCAL_CHAT_CANCELLED:AW_LOCAL_CHAT_FAILED,
        expected==UMI_STATUS_OK?UMI_AI_RESPONSE_STREAM_COMPLETE:
        expected==UMI_STATUS_CANCELLED?UMI_AI_RESPONSE_STREAM_CANCELLED:UMI_AI_RESPONSE_STREAM_FAILED,
        text,expected);
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!UmiAiWorkspaceLocalProviderAvailable()){
        fputs("SKIP: native HTTP/JSON capability unavailable.\n",stderr);return 77;
    }
    const char *mode=argv[1],*body=TEXT_HELLO STOP DONE,*text="Hello";
    const char *type="Content-Type: text/event-stream\r\n",*status_line="HTTP/1.1 200 OK\r\n";
    UmiStatus expected=UMI_STATUS_OK;bool chunked=false,short_body=false;
    uint32_t delay=0U,timeout=1000U;size_t fragment=16384U;
    if(strcmp(mode,"fragmented")==0)fragment=1U;
    else if(strcmp(mode,"chunked")==0)chunked=true;
    else if(strcmp(mode,"informational")==0)status_line="HTTP/1.1 100 Continue\r\n\r\nHTTP/1.1 200 OK\r\n";
    else if(strcmp(mode,"wrong-type")==0){type="Content-Type: application/json\r\n";expected=UMI_STATUS_UNAVAILABLE;text="";}
    else if(strcmp(mode,"missing-type")==0){type="";expected=UMI_STATUS_UNAVAILABLE;text="";}
    else if(strcmp(mode,"status-busy")==0){status_line="HTTP/1.1 503 Unavailable\r\n";expected=UMI_STATUS_UNAVAILABLE;text="";}
    else if(strcmp(mode,"redirect")==0){status_line="HTTP/1.1 302 Found\r\nLocation: http://127.0.0.1:1/forbidden\r\n";expected=UMI_STATUS_UNAVAILABLE;text="";}
    else if(strcmp(mode,"bad-json")==0){body=TEXT_HELLO "data: broken\n\n";expected=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"missing-done")==0){body=TEXT_HELLO STOP;expected=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"duplicate-done")==0){body=TEXT_HELLO STOP DONE DONE;expected=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(mode,"truncated-http")==0){short_body=true;expected=UMI_STATUS_IO_ERROR;}
    else if(strcmp(mode,"timeout")==0){delay=300U;timeout=100U;expected=UMI_STATUS_TIMEOUT;text="";}
    else if(strcmp(mode,"cancel-after-request")==0){expected=UMI_STATUS_CANCELLED;text="";}
    else if(strcmp(mode,"proxy-bypass")==0){if(PoisonProxyEnvironment()!=0)return 1;}
    else if(strcmp(mode,"success")!=0)return 2;

    /* All borrowed worker storage is heap-owned until the peer has joined.
     * A join/release error returns from the process without freeing that storage:
     * a still-live worker must never observe freed response or cancellation data. */
    char *response=malloc(8192U),*encoded=NULL;
    UmiAiRequest *request=calloc(1U,sizeof *request);
    UmiWebRequest *captured=calloc(1U,sizeof *captured);
    UmiAiWorkspaceCancellation *cancellation=NULL;
    AwLocalChatStream *stream=NULL;LocalHttpPeer *peer=NULL;
    int result=1;bool curl_ready=false;
    if(response==NULL || request==NULL || captured==NULL)goto cleanup;
    int bytes;
    if(chunked){
        bytes=snprintf(response,8192U,"%s%sTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n%zx\r\n%s\r\n0\r\nX-Result: finished\r\n\r\n",
            status_line,type,strlen(body),body);
    }else{
        bytes=snprintf(response,8192U,"%s%sContent-Length: %zu\r\nConnection: close\r\n\r\n%s",
            status_line,type,strlen(body)+(short_body?1U:0U),body);
    }
    if(bytes<0 || bytes>=8192)goto cleanup;
    strcpy(request->request_id,"sequence.test");strcpy(request->model_id,"test-model");
    request->message_count=1U;request->max_output_tokens=64U;
    request->messages[0].role=UMI_AI_ROLE_USER;
    strcpy(request->messages[0].text,"Explain quoted \"C\" and a newline:\n");
    if(AwLocalChatEncodeStreaming(request,&encoded)!=UMI_STATUS_OK)goto cleanup;
    if(CreateStream(64U,16384U,&stream)!=UMI_STATUS_OK)goto cleanup;
    if(UmiAiWorkspaceCancellationCreate(&cancellation)!=UMI_STATUS_OK)goto cleanup;
    if(curl_global_init(CURL_GLOBAL_DEFAULT)!=CURLE_OK)goto cleanup;
    curl_ready=true;
    LocalHttpPeerOptions options={.response=response,.response_bytes=(size_t)bytes,
        .fragment_bytes=fragment,.delay_millis=delay,.allow_disconnect=expected!=UMI_STATUS_OK,
        .cancel_after_request=strcmp(mode,"cancel-after-request")==0?cancellation:NULL};
    uint16_t port=0U;
    if(LocalHttpPeerStart(&options,&peer,&port)!=UMI_STATUS_OK)goto cleanup;
    AwLocalHttpStreamResult metadata;
    UmiStatus status=AwLocalChatExchangeStream(port,timeout,encoded,16384U,cancellation,stream,&metadata);
    UmiStatus joined=LocalHttpPeerFinish(&peer,captured);
    if(peer!=NULL){fputs("Peer join/release failed; borrowed storage retained until process exit.\n",stderr);return 1;}
    if(joined!=UMI_STATUS_OK){fputs("Peer transport or cleanup failed.\n",stderr);goto cleanup;}
    if(CheckRequest(captured,encoded)!=0)goto cleanup;
    if(CheckResult(stream,&metadata,status,expected,text)!=0)goto cleanup;
    if(expected==UMI_STATUS_OK && metadata.response.body_bytes!=strlen(body))goto cleanup;
    if(chunked && !metadata.response.chunked)goto cleanup;
    if(strcmp(mode,"informational")==0 && metadata.response.informational_responses!=1U)goto cleanup;
    if(strcmp(mode,"status-busy")==0 && metadata.response.http_status!=503U)goto cleanup;
    if(strcmp(mode,"redirect")==0 && metadata.response.http_status!=302U)goto cleanup;
    result=0;
cleanup:
    if(curl_ready)curl_global_cleanup();
    AwLocalChatStreamDestroy(stream);UmiAiWorkspaceCancellationDestroy(cancellation);
    AwLocalChatFree(encoded);free(response);free(request);free(captured);return result;
}
