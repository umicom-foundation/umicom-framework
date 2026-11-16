/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/local_http_peer.c
 *
 * PURPOSE:
 *   Run one bounded native loopback peer while reusing Framework request framing and thread ownership.
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
#include "local_http_peer.h"
#include "umicom/platform/threading.h"
#include "umicom/web/request_stream.h"
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef SOCKET PeerSocket;
#define PEER_BAD INVALID_SOCKET
static int CloseSocket(PeerSocket socket){return closesocket(socket);}
static bool Again(void){int error=WSAGetLastError();return error==WSAEWOULDBLOCK || error==WSAEINTR;}
static uint64_t Milliseconds(void){return (uint64_t)GetTickCount64();}
/* Make the Winsock command-mask conversion explicit at its native boundary. The former implementation is retained for owner review. */
#if 0
static bool Nonblocking(PeerSocket socket){u_long enabled=1UL;return ioctlsocket(socket,FIONBIO,&enabled)==0;}
#endif
static bool Nonblocking(PeerSocket socket)
{
    u_long enabled=1UL;
    /* Winsock declares its 32-bit command mask as signed long. The explicit
     * conversion preserves the Windows ABI bit pattern of FIONBIO. */
    return ioctlsocket(socket,(long)FIONBIO,&enabled)==0;
}
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
typedef int PeerSocket;
#define PEER_BAD (-1)
static int CloseSocket(PeerSocket socket){return close(socket);}
static bool Again(void){return errno==EAGAIN || errno==EWOULDBLOCK || errno==EINTR;}
static uint64_t Milliseconds(void)
{
    struct timespec value;
    if(clock_gettime(CLOCK_MONOTONIC,&value)!=0)return 0U;
    return (uint64_t)value.tv_sec*1000U+(uint64_t)value.tv_nsec/1000000U;
}
static bool Nonblocking(PeerSocket socket)
{
    if(socket>=FD_SETSIZE)return false;
    int flags=fcntl(socket,F_GETFL,0);return flags>=0 && fcntl(socket,F_SETFL,flags|O_NONBLOCK)==0;
}
#endif
struct LocalHttpPeer {
    PeerSocket listener;
    UmiThread *thread;
    UmiWebRequest *request;
    LocalHttpPeerOptions options;
    bool joined;
    atomic_bool request_received;
    int worker_result;
#ifdef _WIN32
    bool winsock;
#endif
};
static bool WaitSocket(PeerSocket socket,bool writing,uint64_t deadline)
{
    for(;;){
        uint64_t now=Milliseconds();if(now==0U || now>=deadline)return false;
        fd_set ready;FD_ZERO(&ready);FD_SET(socket,&ready);
        struct timeval timeout={0,100000};
#ifdef _WIN32
        int status=select(0,writing?NULL:&ready,writing?&ready:NULL,NULL,&timeout);
#else
        int status=select(socket+1,writing?NULL:&ready,writing?&ready:NULL,NULL,&timeout);
#endif
        if(status>0)return true;
        if(status<0 && !Again())return false;
    }
}
static int PeerMain(void *opaque)
{
    LocalHttpPeer *peer=opaque;
    uint64_t now=Milliseconds();if(now==0U)return 1;
    uint64_t deadline=now+5000U;PeerSocket client=PEER_BAD;
    while(client==PEER_BAD){
        if(!WaitSocket(peer->listener,false,deadline))return 1;
        client=accept(peer->listener,NULL,NULL);
        if(client==PEER_BAD && !Again())return 1;
    }
    int result=1;UmiWebRequestStream *request=NULL;
    if(!Nonblocking(client))goto cleanup;
#ifdef SO_NOSIGPIPE
    int no_signal=1;
    if(setsockopt(client,SOL_SOCKET,SO_NOSIGPIPE,&no_signal,sizeof no_signal)!=0)goto cleanup;
#endif
    if(UmiWebRequestStreamCreate(16384U,&request)!=UMI_STATUS_OK)goto cleanup;
    for(;;){
        if(!WaitSocket(client,false,deadline))goto cleanup;
        char buffer[2048];
#ifdef _WIN32
        int bytes=recv(client,buffer,(int)sizeof buffer,0);
#else
        ssize_t bytes=recv(client,buffer,sizeof buffer,0);
#endif
        if(bytes<0){if(Again())continue;goto cleanup;}
        if(bytes==0)goto cleanup;
        UmiStatus status=UmiWebRequestStreamFeed(request,buffer,(size_t)bytes);
        if(status==UMI_STATUS_BUSY)continue;
        if(status!=UMI_STATUS_OK || UmiWebRequestStreamParse(request,peer->request)!=UMI_STATUS_OK)goto cleanup;
        break;
    }
    atomic_store_explicit(&peer->request_received,true,memory_order_release);
    if(peer->options.cancel_after_request!=NULL){
        UmiAiWorkspaceCancellationRequest(peer->options.cancel_after_request);result=0;goto cleanup;
    }
    if(peer->options.delay_millis!=0U)umi_thread_sleep_ms(peer->options.delay_millis);
    size_t offset=0U;
    while(offset<peer->options.response_bytes){
        if(!WaitSocket(client,true,deadline))goto cleanup;
        size_t amount=peer->options.response_bytes-offset;
        if(amount>peer->options.fragment_bytes)amount=peer->options.fragment_bytes;
#ifdef _WIN32
        int sent=send(client,peer->options.response+offset,(int)amount,0);
#else
        int flags=0;
#ifdef MSG_NOSIGNAL
        flags=MSG_NOSIGNAL;
#endif
        ssize_t sent=send(client,peer->options.response+offset,amount,flags);
#endif
        if(sent<0){
            if(Again())continue;
            if(peer->options.allow_disconnect)result=0;
            goto cleanup;
        }
        if(sent==0){if(peer->options.allow_disconnect)result=0;goto cleanup;}
        offset+=(size_t)sent;
    }
    result=0;
cleanup:
    UmiWebRequestStreamDestroy(request);
    if(CloseSocket(client)!=0)result=1;
    return result;
}
/* Report test-peer native cleanup failures rather than hiding them; startup failures retain their first cause. The former implementation is retained for owner review. */
#if 0
static void ReleasePeer(LocalHttpPeer *peer)
{
    if(peer->listener!=PEER_BAD)(void)CloseSocket(peer->listener);
#ifdef _WIN32
    if(peer->winsock)(void)WSACleanup();
#endif
    free(peer->request);free(peer);
}

#endif
static UmiStatus ReleasePeer(LocalHttpPeer *peer)
{
    UmiStatus status=UMI_STATUS_OK;
    if(peer->listener!=PEER_BAD && CloseSocket(peer->listener)!=0)status=UMI_STATUS_IO_ERROR;
#ifdef _WIN32
    if(peer->winsock && WSACleanup()!=0)status=UMI_STATUS_IO_ERROR;
#endif
    free(peer->request);free(peer);return status;
}
UmiStatus LocalHttpPeerStart(const LocalHttpPeerOptions *options,LocalHttpPeer **out,uint16_t *out_port)
{
    if(options==NULL || out==NULL || out_port==NULL ||
        (options->response==NULL && options->response_bytes!=0U) ||
        options->fragment_bytes==0U || options->fragment_bytes>16384U ||
        options->delay_millis>1000U)return UMI_STATUS_INVALID_ARGUMENT;
    LocalHttpPeer *peer=calloc(1U,sizeof *peer);if(peer==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    peer->listener=PEER_BAD;peer->options=*options;
    atomic_init(&peer->request_received,false);
    peer->request=calloc(1U,sizeof *peer->request);
    if(peer->request==NULL){ReleasePeer(peer);return UMI_STATUS_OUT_OF_MEMORY;}
#ifdef _WIN32
    WSADATA winsock;
    if(WSAStartup(MAKEWORD(2,2),&winsock)!=0){ReleasePeer(peer);return UMI_STATUS_UNAVAILABLE;}
    peer->winsock=true;
#endif
    peer->listener=socket(AF_INET,SOCK_STREAM,0);
    if(peer->listener==PEER_BAD || !Nonblocking(peer->listener)){ReleasePeer(peer);return UMI_STATUS_IO_ERROR;}
    struct sockaddr_in address={0};address.sin_family=AF_INET;address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    if(bind(peer->listener,(struct sockaddr *)&address,sizeof address)!=0 || listen(peer->listener,1)!=0){
        ReleasePeer(peer);return UMI_STATUS_IO_ERROR;
    }
#ifdef _WIN32
    int address_bytes=(int)sizeof address;
#else
    socklen_t address_bytes=(socklen_t)sizeof address;
#endif
    if(getsockname(peer->listener,(struct sockaddr *)&address,&address_bytes)!=0){
        ReleasePeer(peer);return UMI_STATUS_IO_ERROR;
    }
    uint16_t port=ntohs(address.sin_port);
    if(port<1024U){ReleasePeer(peer);return UMI_STATUS_UNAVAILABLE;}
    UmiStatus status=umi_thread_start(PeerMain,peer,&peer->thread);
    if(status!=UMI_STATUS_OK){ReleasePeer(peer);return status;}
    *out=peer;*out_port=port;return UMI_STATUS_OK;
}
UmiStatus LocalHttpPeerFinish(LocalHttpPeer **in_out,UmiWebRequest *out_request)
{
    if(in_out==NULL || *in_out==NULL || out_request==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    LocalHttpPeer *peer=*in_out;
    if(!peer->joined){
        UmiStatus status=umi_thread_join(peer->thread,&peer->worker_result);
        if(status!=UMI_STATUS_OK)return status;
        peer->joined=true;
    }
    UmiStatus status=UmiThreadRelease(&peer->thread);
    if(status!=UMI_STATUS_OK)return status;
/* Keep captured output unchanged until both worker and native cleanup have completed successfully. The former implementation is retained for owner review. */
#if 0
    status=peer->worker_result==0?UMI_STATUS_OK:UMI_STATUS_IO_ERROR;
    if(status==UMI_STATUS_OK)*out_request=*peer->request;
    ReleasePeer(peer);*in_out=NULL;return status;
#endif
    status=peer->worker_result==0?UMI_STATUS_OK:UMI_STATUS_IO_ERROR;
    UmiWebRequest *captured=peer->request;peer->request=NULL;
    UmiStatus released=ReleasePeer(peer);*in_out=NULL;
    if(status==UMI_STATUS_OK)status=released;
    if(status==UMI_STATUS_OK)*out_request=*captured;
    free(captured);return status;
}

bool LocalHttpPeerHasRequest(const LocalHttpPeer *peer)
{
    return peer!=NULL && atomic_load_explicit(&peer->request_received,memory_order_acquire);
}
