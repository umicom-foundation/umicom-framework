/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_stream_loopback.c
 *
 * PURPOSE:
 *   Exercise public background stream ownership through real loopback HTTP and joined peer cleanup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/local_stream.h"
#include "umicom/platform/threading.h"
#include "local_http_peer.h"
#include "local_chat_stream_fixture.h"
static int AwaitRequest(LocalHttpPeer *peer)
{
    for(size_t i=0U;i<2000U;++i){
        if(LocalHttpPeerHasRequest(peer))return 0;
        umi_thread_sleep_ms(1U);
    }
    return 1;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *mode=argv[1];UmiStatus expected=UMI_STATUS_OK;uint32_t delay=0U,timeout=1000U;
    if(strcmp(mode,"cancel")==0){expected=UMI_STATUS_CANCELLED;delay=300U;}
    else if(strcmp(mode,"timeout")==0){expected=UMI_STATUS_TIMEOUT;delay=300U;timeout=100U;}
    else if(strcmp(mode,"truncated")==0)expected=UMI_STATUS_IO_ERROR;
    else if(strcmp(mode,"success")!=0)return 2;
    UmiAiLocalTransport *runtime=NULL;UmiStatus status=UmiAiLocalTransportCreate(&runtime);
    if(status==UMI_STATUS_UNAVAILABLE){fputs("SKIP: native thread-safe HTTP runtime unavailable.\n",stderr);return 77;}
    CHECK(status==UMI_STATUS_OK);
    /* Heap storage remains owned until both independently started workers have
     * joined. Any failed CHECK ends this test process without freeing live data. */
    char *response=malloc(4096U);CHECK(response!=NULL);
    int bytes=snprintf(response,4096U,"HTTP/1.1 200 OK\r\nContent-Type: text/event-stream\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n%s",
        strlen(TEXT_HELLO STOP DONE)+(strcmp(mode,"truncated")==0?1U:0U),TEXT_HELLO STOP DONE);
    CHECK(bytes>0 && bytes<4096);
    LocalHttpPeerOptions peer_options={.response=response,.response_bytes=(size_t)bytes,
        .fragment_bytes=16384U,.delay_millis=delay,.allow_disconnect=expected!=UMI_STATUS_OK};
    LocalHttpPeer *peer=NULL;uint16_t port=0U;
    CHECK(LocalHttpPeerStart(&peer_options,&peer,&port)==UMI_STATUS_OK);
    UmiAiRequest *request=calloc(1U,sizeof *request);CHECK(request!=NULL);
    strcpy(request->request_id,"sequence.test");strcpy(request->model_id,"test-model");
    request->message_count=1U;request->max_output_tokens=64U;
    request->messages[0].role=UMI_AI_ROLE_USER;strcpy(request->messages[0].text,"Explain native requests");
    UmiAiLocalStreamOptions options={.port=port,.timeout_millis=timeout,.maximum_text_bytes=64U,
        .maximum_wire_bytes=16384U,.identity={17U,29U},.allow_network=true};
    UmiAiLocalStream *job=NULL;
    CHECK(UmiAiLocalStreamCreate(runtime,request,&options,&job)==UMI_STATUS_OK);
    memset(request,0xa5,sizeof *request);free(request);
    CHECK(UmiAiLocalStreamStart(job)==UMI_STATUS_OK);
    if(strcmp(mode,"cancel")==0){
        CHECK(AwaitRequest(peer)==0);CHECK(UmiAiLocalStreamCancel(job)==UMI_STATUS_OK);
    }
    CHECK(UmiAiLocalStreamJoin(job)==UMI_STATUS_OK);
    UmiWebRequest *captured=calloc(1U,sizeof *captured);CHECK(captured!=NULL);
    CHECK(LocalHttpPeerFinish(&peer,captured)==UMI_STATUS_OK && peer==NULL);
    CHECK(captured->method==UMI_HTTP_METHOD_POST && strcmp(captured->path,"/v1/chat/completions")==0);
    UmiAiLocalStreamResult result;
    CHECK(UmiAiLocalStreamReadResult(job,&result)==UMI_STATUS_OK);
    if(result.operation_status!=expected){
        fprintf(stderr,"Observed %d; expected %d\n",(int)result.operation_status,(int)expected);return 1;
    }
    CHECK(result.observation_status==UMI_STATUS_OK);
    char *copy=NULL;UmiAiResponseStreamSnapshot snapshot;
    CHECK(UmiAiLocalStreamCopyText(job,&copy,&snapshot)==UMI_STATUS_OK);
    const char *text=(expected==UMI_STATUS_OK || expected==UMI_STATUS_IO_ERROR)?"Hello":"";
    CHECK(strcmp(copy,text)==0);
    CHECK(snapshot.state==(expected==UMI_STATUS_OK?UMI_AI_RESPONSE_STREAM_COMPLETE:
        expected==UMI_STATUS_CANCELLED?UMI_AI_RESPONSE_STREAM_CANCELLED:UMI_AI_RESPONSE_STREAM_FAILED));
    CHECK(UmiAiLocalTransportDestroy(&runtime)==UMI_STATUS_BUSY);
    CHECK(UmiAiLocalStreamDestroy(&job)==UMI_STATUS_OK && job==NULL);
    CHECK(strcmp(copy,text)==0); /* Copied output outlives the request owner. */
    UmiAiResponseStreamTextFree(copy);free(captured);free(response);
    CHECK(UmiAiLocalTransportDestroy(&runtime)==UMI_STATUS_OK);return 0;
}
