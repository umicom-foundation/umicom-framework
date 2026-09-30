/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_evidence/test_native_http.c
 * PURPOSE:
 *   Native loopback peer for the actual local-provider and saved-job path. No model is
 *   loaded. No credential, external address or real tool is used. The previous Python
 *   fixture remains available as an alternative test.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native loopback peer for the actual local-provider and saved-job path.
 * No model is loaded. No credential, external address or real tool is used.
 * The previous Python fixture remains available as an alternative test.
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef SOCKET PeerSocket;
#define BAD_SOCKET INVALID_SOCKET
#else
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <time.h>
typedef int PeerSocket;
#define BAD_SOCKET (-1)
#endif
#include "fixture.h"
#include <limits.h>

typedef struct Peer {
    PeerSocket listener;
    const char *mode;
    UmiAiWorkspaceCancellation *cancel;
    int result;
} Peer;
static void PeerClose(PeerSocket socketValue)
{
#ifdef _WIN32
    closesocket(socketValue);
#else
    close(socketValue);
#endif
}
static void SleepMillis(unsigned milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);
#else
    struct timespec delay={(time_t)(milliseconds/1000U),(long)(milliseconds%1000U)*1000000L};
    while(nanosleep(&delay,&delay)!=0 && errno==EINTR) { }
#endif
}
static int WaitRead(PeerSocket socketValue)
{
    fd_set set;FD_ZERO(&set);FD_SET(socketValue,&set);struct timeval time={3,0};
#ifdef _WIN32
    return select(0,&set,NULL,NULL,&time);
#else
    return select(socketValue+1,&set,NULL,NULL,&time);
#endif
}
static bool SendAll(PeerSocket socketValue,const char *text,size_t count)
{
    for(size_t offset=0U;offset<count;){
        size_t part=count-offset;if(part>31U)part=31U;
#ifdef _WIN32
        int n=send(socketValue,text+offset,(int)part,0);
#else
        ssize_t n=send(socketValue,text+offset,part,MSG_NOSIGNAL);
#endif
        if(n<=0)return false;
        offset+=(size_t)n;
    }return true;
}
static int Serve(Peer *peer)
{
    if(WaitRead(peer->listener)!=1)return 1;
    PeerSocket client=accept(peer->listener,NULL,NULL);
    if(client==BAD_SOCKET)return 1;
#ifndef _WIN32
    if(client>=FD_SETSIZE){PeerClose(client);return 1;}
#endif
    char request[20000];size_t used=0U,needed=SIZE_MAX;bool received=false;
    while(used+1U<sizeof request){
        if(WaitRead(client)!=1)break;
#ifdef _WIN32
        int n=recv(client,request+used,(int)(sizeof request-used-1U),0);
#else
        ssize_t n=recv(client,request+used,sizeof request-used-1U,0);
#endif
        if(n<=0)break;
        used+=(size_t)n;request[used]='\0';
        char *end=strstr(request,"\r\n\r\n");
        if(end!=NULL&&needed==SIZE_MAX){
            char *length=strstr(request,"Content-Length:");
            if(length==NULL||length>end)break;
            char *last=NULL;unsigned long body=strtoul(length+15,&last,10);
            if(last==length+15 || *last!='\r'||body>=sizeof request)break;
            needed=(size_t)(end+4-request)+(size_t)body;
        }
        if(needed!=SIZE_MAX&&used>=needed){received=true;break;}
    }
    if(!received || strncmp(request,"POST /v1/chat/completions HTTP/1.1\r\n",35U)!=0 ||
       strstr(request,"local-test")==NULL || strstr(request,"Source [S1]")==NULL ||
       strstr(request,"\"tools\"")!=NULL){PeerClose(client);return 1;}
    if(strcmp(peer->mode,"disconnect")==0){PeerClose(client);return 0;}
    if(strcmp(peer->mode,"timeout")==0){SleepMillis(700U);PeerClose(client);return 0;}
    if(strcmp(peer->mode,"cancel")==0){UmiAiWorkspaceCancellationRequest(peer->cancel);SleepMillis(700U);PeerClose(client);return 0;}
    const char *body="{\"model\":\"local-test\",\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"Checkpoint first. [S1]\"},\"finish_reason\":\"stop\"}]}";
    if(strcmp(peer->mode,"unicode")==0)body="{\"model\":\"local-test\",\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"مرحبا 世界 [S1]\"},\"finish_reason\":\"stop\"}]}";
    if(strcmp(peer->mode,"model_mismatch")==0)body="{\"model\":\"other\",\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"[S1]\"},\"finish_reason\":\"stop\"}]}";
    if(strcmp(peer->mode,"tool_call")==0)body="{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"[S1]\",\"tool_calls\":[]},\"finish_reason\":\"stop\"}]}";
    if(strcmp(peer->mode,"bad_citation")==0)body="{\"choices\":[{\"message\":{\"role\":\"assistant\",\"content\":\"[S9]\"},\"finish_reason\":\"stop\"}]}";
    if(strcmp(peer->mode,"bad_json")==0)body="{";
    char header[400];int code=strcmp(peer->mode,"busy")==0?503:strcmp(peer->mode,"redirect")==0?302:200;
    int count=snprintf(header,sizeof header,"HTTP/1.1 %d Fixture\r\nContent-Type: application/json\r\nContent-Length: %zu\r\nConnection: close\r\nLocation: http://127.0.0.1:1/not-followed\r\n\r\n",code,strlen(body));
    bool sent=count>0&&(size_t)count<sizeof header&&SendAll(client,header,(size_t)count)&&SendAll(client,body,strlen(body));
    PeerClose(client);return sent?0:1;
}
#ifdef _WIN32
static DWORD WINAPI PeerThread(void *context){Peer *peer=context;peer->result=Serve(peer);return 0;}
#else
static void *PeerThread(void *context){Peer *peer=context;peer->result=Serve(peer);return NULL;}
#endif
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
/* Explain unavailable native transport without reporting a skipped check as a pass. The previous implementation remains for engineering review. */
#if 0
    if(!UmiAiWorkspaceLocalProviderAvailable())return 77;
#endif
    if (!UmiAiWorkspaceLocalProviderAvailable()) {
        fputs("SKIP: local AI HTTP was not built. Enable UMICOM_AI_WORKSPACE_LOCAL_HTTP=ON with libcurl and json-c installed. No provider request was made.\n", stderr);
        return 77;
    }
    Fixture f;CHECK(OpenFixture(&f,NULL)==0);
    UmiAiWorkspaceCancellation *cancel=NULL;OK(UmiAiWorkspaceCancellationCreate(&cancel));
#ifdef _WIN32
    WSADATA wsa;CHECK(WSAStartup(MAKEWORD(2,2),&wsa)==0);
#endif
    Peer peer={socket(AF_INET,SOCK_STREAM,IPPROTO_TCP),argv[1],cancel,1};CHECK(peer.listener!=BAD_SOCKET);
#ifndef _WIN32
    CHECK(peer.listener<FD_SETSIZE);
#endif
    struct sockaddr_in address;memset(&address,0,sizeof address);address.sin_family=AF_INET;
    address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);address.sin_port=0;
    CHECK(bind(peer.listener,(struct sockaddr *)&address,(int)sizeof address)==0);CHECK(listen(peer.listener,1)==0);
#ifdef _WIN32
    int length=(int)sizeof address;
#else
    socklen_t length=sizeof address;
#endif
    CHECK(getsockname(peer.listener,(struct sockaddr *)&address,&length)==0);
    uint16_t port=ntohs(address.sin_port);CHECK(port>=1024U);
    UmiAiProvider provider;OK(UmiAiWorkspaceLocalProviderCreate(port,400U,cancel,&provider));
    OK(umi_ai_provider_registry_add(&f.runtime.providers,&provider));
    OK(UmiAiWorkspacePrepareEvidence(f.workspace,"network","umicom.local-chat","local-test",
        "notes","Explain checkpoints.","writer",256U,f.snapshot.corpusRevision,f.evidence,1U));
    OK(UmiAiWorkspaceReview(f.workspace,"network","reviewer",true));
#ifdef _WIN32
    HANDLE thread=CreateThread(NULL,0,PeerThread,&peer,0,NULL);CHECK(thread!=NULL);
#else
    pthread_t thread;CHECK(pthread_create(&thread,NULL,PeerThread,&peer)==0);
#endif
    UmiStatus result=UmiAiWorkspaceRun(f.workspace,"network",cancel);
#ifdef _WIN32
    CHECK(WaitForSingleObject(thread,10000U)==WAIT_OBJECT_0);CloseHandle(thread);
#else
    CHECK(pthread_join(thread,NULL)==0);
#endif
    PeerClose(peer.listener);
    UmiStatus expected=UMI_STATUS_OK;
    if(strcmp(peer.mode,"timeout")==0)expected=UMI_STATUS_TIMEOUT;
    if(strcmp(peer.mode,"cancel")==0)expected=UMI_STATUS_CANCELLED;
    if(strcmp(peer.mode,"model_mismatch")==0||strcmp(peer.mode,"bad_json")==0||strcmp(peer.mode,"bad_citation")==0)expected=UMI_STATUS_PARSE_ERROR;
    if(strcmp(peer.mode,"tool_call")==0)expected=UMI_STATUS_PERMISSION_DENIED;
    if(strcmp(peer.mode,"busy")==0)expected=UMI_STATUS_BUSY;
    if(strcmp(peer.mode,"redirect")==0||strcmp(peer.mode,"disconnect")==0)expected=UMI_STATUS_IO_ERROR;
    CHECK(peer.result==0);CHECK(result==expected);
    UmiAiWorkspaceJob job;OK(UmiAiWorkspaceJobFind(f.workspace,"network",&job));
    if(expected==UMI_STATUS_OK){
        CHECK(job.state==UMI_AI_WORKSPACE_SUCCEEDED);
        UmiAiEvidenceReview *review=NULL;OK(UmiAiEvidenceCapture(f.workspace,"network",&review));
        UmiAiEvidenceSummary summary;OK(UmiAiEvidenceSummaryRead(review,&summary));CHECK(summary.validReferences==1U);
        UmiAiEvidenceDestroy(review);
    }else CHECK(job.response.text[0]=='\0'&&job.state!=UMI_AI_WORKSPACE_SUCCEEDED);
    CloseFixture(&f);UmiAiWorkspaceCancellationDestroy(cancel);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
